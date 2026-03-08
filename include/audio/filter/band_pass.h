//
// Created by skaldi on 08.03.26.
//

#pragma once
#include "filter_interface.h"

namespace age::audio::filter
{
    class band_pass : public filter_interface
    {
    public:

    public:
        void set_gain(float value);
        float get_gain() const;

        void set_gain_lf(float value);
        float get_gain_lf() const;

        void set_gain_hf(float value);
        float get_gain_hf() const;
    protected:
        int32_t get_type() const override;

    private:
    };
}
