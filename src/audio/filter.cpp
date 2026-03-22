//
// Created by skaldi on 22.03.26.
//

#include "audio/filter.h"

#include <array>

#define AL_ALEXT_PROTOTYPES
#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio
{
    void filter::apply_settings(const settings& value, bool forced)
    {
        update_driver(value, forced);
        m_settings = value;
    }

    void filter::update_driver(const settings& value, bool forced) const
    {
        auto mode_to_al_int = [](mode value) -> ALint
        {
            static constexpr std::array al_filter_types{
                AL_FILTER_NULL,
                AL_FILTER_LOWPASS,
                AL_FILTER_HIGHPASS,
                AL_FILTER_BANDPASS
            };

            ALint al_mode = AL_FILTER_NULL;
            if (auto index = static_cast<uint8_t>(value); index < al_filter_types.size())
                al_mode = al_filter_types[index];

            return al_mode;
        };

        if (forced || m_settings.type != value.type)
        {
            if (realize()) AL_CALL(alFilteri(m_handle, AL_FILTER_TYPE, mode_to_al_int(value.type)));
            forced = true;
        }

        switch (value.type)
        {
            case mode::low_pass:
                if (forced || m_settings.gain != value.gain)        if (realize()) AL_CALL(alFilterf(m_handle, AL_LOWPASS_GAIN, value.gain));
                if (forced || m_settings.gain_hf != value.gain_hf)  if (realize()) AL_CALL(alFilterf(m_handle, AL_LOWPASS_GAINHF, value.gain_hf));
                break;

            case mode::high_pass:
                if (forced || m_settings.gain != value.gain)        if (realize()) AL_CALL(alFilterf(m_handle, AL_HIGHPASS_GAIN, value.gain));
                if (forced || m_settings.gain_lf != value.gain_lf)  if (realize()) AL_CALL(alFilterf(m_handle, AL_HIGHPASS_GAINLF, value.gain_lf));
                break;

            case mode::band_pass:
                if (forced || m_settings.gain != value.gain)        if (realize()) AL_CALL(alFilterf(m_handle, AL_BANDPASS_GAIN, value.gain));
                if (forced || m_settings.gain_hf != value.gain_hf)  if (realize()) AL_CALL(alFilterf(m_handle, AL_BANDPASS_GAINHF, value.gain_hf));
                if (forced || m_settings.gain_lf != value.gain_lf)  if (realize()) AL_CALL(alFilterf(m_handle, AL_BANDPASS_GAINLF, value.gain_lf));
                break;

            default: break;
        }
    }

    bool filter::realize() const
    {
        if (m_handle == 0)
        {
            m_handle = gen_handle();
            if (m_handle) init_internal();
        }

        return m_handle != 0;
    }

    void filter::init_internal() const
    {
        update_driver(m_settings, true);
    }

    uint32_t filter::gen_handle()
    {
        ALuint handle{};
        AL_CALL(alGenFilters(1, &handle));

        return handle;
    }

    void filter::delete_handle(uint32_t handle)
    {
        AL_CALL(alDeleteFilters(1, &handle));
    }
}
