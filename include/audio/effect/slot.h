//
// Created by skaldi on 06.03.26.
//

#pragma once

#include <mutex>

namespace age::audio
{
    class filter; // Forward declaration
}

namespace age::audio::effect
{
    class effect_interface;
    class slot
    {
    public:
        friend class effect_interface;
        friend class audio::filter;

        slot() = default;
        ~slot();

        // --- Copying is forbidden due to mutex ownership ---
        slot(const slot&) = delete;
        slot& operator=(const slot&) = delete;

        slot(slot&& other) noexcept;
        slot& operator=(slot&& other) noexcept;

    public:
        void attach_effect(effect_interface* value);
        void attach_filter(filter* value);

        effect_interface* get_effect() const;
        filter* get_filter() const;

        void set_volume(float value);
        float get_volume() const;

        void apply_effect();
        void apply_filter();

    protected:

    private:
        void notify_death();
        void on_effect_destroyed(effect_interface* value);
        void on_filter_destroyed(filter* value);

        std::mutex m_effect_mutex{};
        effect_interface* m_effect{ nullptr };

        std::mutex m_filter_mutex{};
        filter* m_filter{ nullptr };
    };
}