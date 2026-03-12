//
// Created by skaldi on 06.03.26.
//

#pragma once

#include "audio/effect/effect_interface.h"

namespace age::audio::effect
{
    class compressor : public effect_interface
    {
    public:

    public:
        void set_on_off(bool value);
        bool get_on_off() const;

    protected:
        void init() override;

    private:
        bool m_on_off{ true };
    };
}