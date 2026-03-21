//
// Created by skaldi on 06.03.26.
//

#pragma once

#include <mutex>

#include "utility/utility.h"

namespace age::audio::filter
{
    class filter_interface;
}

namespace age::audio::effect
{
    class effect_interface;
    class auxiliary_send_group;

    class slot
    {
    public:
        friend class auxiliary_send_group;
        friend class effect_interface;
        friend class audio::filter::filter_interface;

        slot(auxiliary_send_group* parent, size_t index)
            : m_owner{ parent }
            , m_index{ index }
        {}
        ~slot();

        // --- Copying is forbidden due to mutex ownership ---
        slot(const slot&) = delete;
        slot& operator=(const slot&) = delete;

        slot(slot&& other) noexcept;
        slot& operator=(slot&& other) noexcept;

    public:
        void set_passthrough();

        void attach_effect(const effect_interface* value);
        void attach_filter(const filter::filter_interface* value);

        const effect_interface* get_effect() const;
        const filter::filter_interface* get_filter() const;

        void set_volume(float value);
        float get_volume() const;

        void apply_effect();

        void set_spatialized(bool value);
        bool get_spatialized() const;

        uint32_t get_handle() const { return m_handle; }

    protected:

    private:
        static uint32_t gen_handle();
        static void delete_handle(uint32_t handle);

        bool ensure_handle();
        void release();

        void notify_death();
        void on_effect_destroyed(const effect_interface* value);
        void on_filter_destroyed(const filter::filter_interface* value);

        std::mutex m_effect_mutex{};
        const effect_interface* m_effect{ nullptr };

        std::mutex m_filter_mutex{};
        const filter::filter_interface* m_filter{ nullptr };

        auxiliary_send_group* m_owner{ nullptr };
        size_t m_index{};

        unique_handle<uint32_t, delete_handle> m_handle{};

        float m_volume { 1.0f };

        bool m_spatialized{ true };
    };
}