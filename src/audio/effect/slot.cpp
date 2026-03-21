//
// Created by skaldi on 06.03.26.
//

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/effect/slot.h"

#include "audio/device.h"

#include "audio/effect/effect_interface.h"
#include "audio/effect/auxiliary_send_group.h"
#include "audio/filter/filter_interface.h"

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    slot::~slot()
    {
        notify_death();
    }

    slot::slot(slot&& other) noexcept
    {
        std::scoped_lock lock(m_effect_mutex, other.m_effect_mutex);
        m_effect = std::exchange(other.m_effect, nullptr);
        if (m_effect)
        {
            // Tell the effect to swap the address in the registry
            m_effect->update_slot_address(&other, this);
        }
    }

    slot& slot::operator=(slot&& other) noexcept
    {
        if (this == &other) return *this;

        std::scoped_lock lock(m_effect_mutex, other.m_effect_mutex);

        if (m_effect) m_effect->remove_slot(this);

        // Transfer the effect
        m_effect = std::exchange(other.m_effect, nullptr);

        if (m_effect)
        {
            // Tell the effect that the pointer has changed from 'other' to 'this'
            // This is safer than removing and adding, because it avoids
            // triggering a "destroyed" signal for a slot that is just moving.
            m_effect->update_slot_address(&other, this);
        }

        return *this;
    }

    void slot::notify_death()
    {
        std::lock_guard lock{ m_effect_mutex };

        if (m_effect) m_effect->remove_slot(this);
    }

    void slot::set_passthrough()
    {
        auto& effect = device::get().get_bus_passthrough();
        auto& filter = device::get().get_mute_filter();

        attach_effect(&effect);
        attach_filter(&filter);
    }

    void slot::attach_effect(const effect_interface *value)
    {
        std::lock_guard lock{ m_effect_mutex };

        if (m_effect == value) return;

        if (m_effect) m_effect->remove_slot(this);

        m_effect = value;

        if (m_effect) m_effect->register_slot(this);
        apply_effect();
    }

    void slot::attach_filter(const filter::filter_interface* value)
    {
        std::lock_guard lock{ m_filter_mutex };

        if (m_filter == value) return;

        if (m_filter) m_filter->remove_slot(this);

        m_filter = value;

        if (m_filter)
        {
            m_filter->register_slot(this);
            (void) m_filter->ensure_handle();
        }

        m_owner->on_slot_filter_changed(*this);
        ensure_handle();
    }

    void slot::apply_effect()
    {
        if (ensure_handle()) alAuxiliaryEffectSloti(m_handle, AL_EFFECTSLOT_EFFECT, m_effect ? static_cast<ALint>(m_effect->get_handle()) : AL_EFFECT_NULL);
    }

    void slot::set_spatialized(bool value)
    {
        if (m_spatialized != value)
        {
            if (ensure_handle()) AL_CALL(alAuxiliaryEffectSloti(m_handle, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, value ? 1 : 0));

            m_spatialized = value;
        }
    }

    bool slot::get_spatialized() const
    {
        return m_spatialized;
    }

    uint32_t slot::gen_handle()
    {
        ALuint slotID;
        AL_CALL(alGenAuxiliaryEffectSlots(1, &slotID));

        return slotID;
    }

    void slot::delete_handle(uint32_t handle)
    {
        AL_CALL(alDeleteAuxiliaryEffectSlots(1, &handle));
    }

    bool slot::ensure_handle()
    {
        if (m_handle == 0)
        {
            m_handle = gen_handle();
            AL_CALL(alAuxiliaryEffectSloti(m_handle, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO,1));
        }

        return m_handle != 0;
    }

    void slot::release()
    {
        if (m_handle) m_handle.reset(0);
    }

    const effect_interface* slot::get_effect() const
    {
        return m_effect;
    }

    const filter::filter_interface* slot::get_filter() const
    {
        return m_filter;
    }

    void slot::set_volume(float value)
    {
        if (m_volume != value)
        {
            if (ensure_handle()) AL_CALL(alAuxiliaryEffectSlotf(m_handle, AL_EFFECTSLOT_GAIN, value));

            m_volume = value;
        }
    }

    float slot::get_volume() const
    {
        return m_volume;
    }

    void slot::on_effect_destroyed(const effect_interface* value)
    {
        std::lock_guard lock{ m_effect_mutex };

        if (m_effect == value)
        {
            m_effect = nullptr;
            apply_effect();
        }
    }

    void slot::on_filter_destroyed(const filter::filter_interface *value)
    {
        std::lock_guard lock{ m_filter_mutex };

        if (m_filter == value) m_filter = nullptr;

    }
}
