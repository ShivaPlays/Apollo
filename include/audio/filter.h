//
// Created by skaldi on 22.03.26.
//

#pragma once

#include <stdint.h>

#include "utility/utility.h"

namespace age::audio
{
    class filter
    {
    public:
        enum class mode : uint8_t
        {
            none,
            low_pass,
            high_pass,
            band_pass
        };

        struct settings
        {
            float gain{ 1.0f };
            float gain_hf{ 1.0f };
            float gain_lf{ 1.0f };

            mode type{ mode::none };

            bool operator==(const settings& other) const
            {
                return (type == other.type && gain == other.gain && gain_hf == other.gain_hf && gain_lf == other.gain_lf);
            }

            bool operator!=(const settings& other) const { return !(*this == other); }
        };

    public:
        void apply_settings(const settings& value, bool forced = false);
        bool realize() const;

    protected:

    private:
        void update_driver(const settings& value, bool forced = false) const;
        void init_internal() const;

        static uint32_t gen_handle();
        static void delete_handle(uint32_t handle);

        settings m_settings;

        mutable unique_handle<uint32_t, delete_handle> m_handle;
    };
}