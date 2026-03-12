//
// Created by skaldi on 08.03.26.
//

#pragma once
#include "filter_interface.h"

namespace age::audio::filter
{
    class low_pass : public filter_interface
    {
    public:

    public:
        void set_gain(float value);
        float get_gain() const;

        void set_gain_hf(float value);
        float get_gain_hf() const;


    protected:
        void init() override;

    private:

        float m_gain{ 1.0f };
        float m_gain_hf{ 1.0f };
    };
}
