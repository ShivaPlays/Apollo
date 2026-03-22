//
// Created by skaldi on 06.03.26.
//

#include "audio/effect/vocal_morpher.h"

#include <array>

#define AL_ALEXT_PROTOTYPES

#include <AL/al.h>
#include <AL/alext.h>

#include "audio/priv/al_check.h"

namespace age::audio::effect
{
    inline ALint phoneme_to_int(vocal_morpher::phoneme value, ALint default_value)
    {
        constexpr auto al_phoneme = std::array
        {
            AL_VOCAL_MORPHER_PHONEME_A,
            AL_VOCAL_MORPHER_PHONEME_E,
            AL_VOCAL_MORPHER_PHONEME_I,
            AL_VOCAL_MORPHER_PHONEME_O,
            AL_VOCAL_MORPHER_PHONEME_U,
            AL_VOCAL_MORPHER_PHONEME_AA,
            AL_VOCAL_MORPHER_PHONEME_AE,
            AL_VOCAL_MORPHER_PHONEME_AH,
            AL_VOCAL_MORPHER_PHONEME_AO,
            AL_VOCAL_MORPHER_PHONEME_EH,
            AL_VOCAL_MORPHER_PHONEME_ER,
            AL_VOCAL_MORPHER_PHONEME_IH,
            AL_VOCAL_MORPHER_PHONEME_IY,
            AL_VOCAL_MORPHER_PHONEME_UH,
            AL_VOCAL_MORPHER_PHONEME_UW,
            AL_VOCAL_MORPHER_PHONEME_B,
            AL_VOCAL_MORPHER_PHONEME_D,
            AL_VOCAL_MORPHER_PHONEME_F,
            AL_VOCAL_MORPHER_PHONEME_G,
            AL_VOCAL_MORPHER_PHONEME_J,
            AL_VOCAL_MORPHER_PHONEME_K,
            AL_VOCAL_MORPHER_PHONEME_L,
            AL_VOCAL_MORPHER_PHONEME_M,
            AL_VOCAL_MORPHER_PHONEME_N,
            AL_VOCAL_MORPHER_PHONEME_P,
            AL_VOCAL_MORPHER_PHONEME_R,
            AL_VOCAL_MORPHER_PHONEME_S,
            AL_VOCAL_MORPHER_PHONEME_T,
            AL_VOCAL_MORPHER_PHONEME_V,
            AL_VOCAL_MORPHER_PHONEME_Z
        };

        if (value < vocal_morpher::phoneme::count) return al_phoneme[static_cast<size_t>(value)];
        return default_value;
    }

    inline vocal_morpher::phoneme int_to_phoneme(ALint value, vocal_morpher::phoneme default_value)
    {
        switch (value)
        {
            case AL_VOCAL_MORPHER_PHONEME_A:    return vocal_morpher::phoneme::a;
            case AL_VOCAL_MORPHER_PHONEME_E:    return vocal_morpher::phoneme::e;
            case AL_VOCAL_MORPHER_PHONEME_I:    return vocal_morpher::phoneme::i;
            case AL_VOCAL_MORPHER_PHONEME_O:    return vocal_morpher::phoneme::o;
            case AL_VOCAL_MORPHER_PHONEME_U:    return vocal_morpher::phoneme::u;
            case AL_VOCAL_MORPHER_PHONEME_AA:   return vocal_morpher::phoneme::aa;
            case AL_VOCAL_MORPHER_PHONEME_AE:   return vocal_morpher::phoneme::ae;
            case AL_VOCAL_MORPHER_PHONEME_AH:   return vocal_morpher::phoneme::ah;
            case AL_VOCAL_MORPHER_PHONEME_AO:   return vocal_morpher::phoneme::ao;
            case AL_VOCAL_MORPHER_PHONEME_EH:   return vocal_morpher::phoneme::eh;
            case AL_VOCAL_MORPHER_PHONEME_ER:   return vocal_morpher::phoneme::er;
            case AL_VOCAL_MORPHER_PHONEME_IH:   return vocal_morpher::phoneme::ih;
            case AL_VOCAL_MORPHER_PHONEME_IY:   return vocal_morpher::phoneme::iy;
            case AL_VOCAL_MORPHER_PHONEME_UH:   return vocal_morpher::phoneme::uh;
            case AL_VOCAL_MORPHER_PHONEME_UW:   return vocal_morpher::phoneme::uw;
            case AL_VOCAL_MORPHER_PHONEME_B:    return vocal_morpher::phoneme::b;
            case AL_VOCAL_MORPHER_PHONEME_D:    return vocal_morpher::phoneme::d;
            case AL_VOCAL_MORPHER_PHONEME_F:    return vocal_morpher::phoneme::f;
            case AL_VOCAL_MORPHER_PHONEME_G:    return vocal_morpher::phoneme::g;
            case AL_VOCAL_MORPHER_PHONEME_J:    return vocal_morpher::phoneme::j;
            case AL_VOCAL_MORPHER_PHONEME_K:    return vocal_morpher::phoneme::k;
            case AL_VOCAL_MORPHER_PHONEME_L:    return vocal_morpher::phoneme::l;
            case AL_VOCAL_MORPHER_PHONEME_M:    return vocal_morpher::phoneme::m;
            case AL_VOCAL_MORPHER_PHONEME_N:    return vocal_morpher::phoneme::n;
            case AL_VOCAL_MORPHER_PHONEME_P:    return vocal_morpher::phoneme::p;
            case AL_VOCAL_MORPHER_PHONEME_R:    return vocal_morpher::phoneme::r;
            case AL_VOCAL_MORPHER_PHONEME_S:    return vocal_morpher::phoneme::s;
            case AL_VOCAL_MORPHER_PHONEME_T:    return vocal_morpher::phoneme::t;
            case AL_VOCAL_MORPHER_PHONEME_V:    return vocal_morpher::phoneme::v;
            case AL_VOCAL_MORPHER_PHONEME_Z:    return vocal_morpher::phoneme::z;
            default: return default_value;
        }
    }

    void vocal_morpher::set_phoneme_a(phoneme value)
    {
        auto phoneme_a = phoneme_to_int(value, 0);

        if (m_phoneme_a != phoneme_a)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_VOCAL_MORPHER_PHONEMEA, phoneme_a));
            m_phoneme_a = phoneme_a;
        }
    }

    vocal_morpher::phoneme vocal_morpher::get_phoneme_a() const
    {
        return int_to_phoneme(m_phoneme_a, phoneme::a);
    }

    void vocal_morpher::set_phoneme_a_coarse_tuning(int32_t value)
    {
        if (m_phoneme_a_coarse_tuning != value)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, value));
            m_phoneme_a_coarse_tuning = value;
        }
    }

    int32_t vocal_morpher::get_phoneme_a_coarse_tuning() const
    {
        return m_phoneme_a_coarse_tuning;
    }

    void vocal_morpher::set_phoneme_b(phoneme value)
    {
        auto phoneme_b = phoneme_to_int(value, 10);

        if (m_phoneme_b != phoneme_b)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_VOCAL_MORPHER_PHONEMEB, phoneme_b));
            m_phoneme_b = phoneme_b;
        }
    }

    vocal_morpher::phoneme vocal_morpher::get_phoneme_b() const
    {
        return int_to_phoneme(m_phoneme_b, phoneme::er);
    }

    void vocal_morpher::set_phoneme_b_coarse_tuning(int32_t value)
    {
        if (m_phoneme_b_coarse_tuning != value)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, value));
            m_phoneme_b_coarse_tuning = value;
        }
    }

    int32_t vocal_morpher::get_phoneme_b_coarse_tuning() const
    {
        return m_phoneme_b_coarse_tuning;
    }

    void vocal_morpher::set_waveform(waveform value)
    {
        constexpr auto al_waveform = std::array{
            AL_VOCAL_MORPHER_WAVEFORM_SINUSOID,
            AL_VOCAL_MORPHER_WAVEFORM_TRIANGLE,
            AL_VOCAL_MORPHER_WAVEFORM_SAWTOOTH
        };

        ALint waveform = AL_VOCAL_MORPHER_WAVEFORM_SINUSOID;
        if (value < waveform::count) waveform = al_waveform[static_cast<size_t>(value)];

        if (m_waveform != waveform)
        {
            if (realize()) AL_CALL(alEffecti(get_handle(), AL_VOCAL_MORPHER_WAVEFORM, waveform));
            m_waveform = waveform;
        }
    }

    vocal_morpher::waveform vocal_morpher::get_waveform() const
    {
        switch (m_waveform)
        {
            case AL_VOCAL_MORPHER_WAVEFORM_SINUSOID: return waveform::sinusoid;
            case AL_VOCAL_MORPHER_WAVEFORM_TRIANGLE: return waveform::triangle;
            case AL_VOCAL_MORPHER_WAVEFORM_SAWTOOTH: return waveform::sawtooth;
            default: return waveform::sinusoid;
        }
    }

    void vocal_morpher::set_rate(float value)
    {
        if (m_rate != value)
        {
            if (realize()) AL_CALL(alEffectf(get_handle(), AL_VOCAL_MORPHER_RATE, value));
            m_rate = value;
        }
    }

    float vocal_morpher::get_rate() const
    {
        return m_rate;
    }

    void vocal_morpher::init() const
    {
        auto handle = get_handle();

        AL_CALL(alEffecti(handle, AL_EFFECT_TYPE, AL_EFFECT_VOCAL_MORPHER));

        AL_CALL(alEffecti(handle, AL_VOCAL_MORPHER_PHONEMEA, m_phoneme_a));
        AL_CALL(alEffecti(handle, AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, m_phoneme_a_coarse_tuning));

        AL_CALL(alEffecti(handle, AL_VOCAL_MORPHER_PHONEMEB, m_phoneme_b));
        AL_CALL(alEffecti(handle, AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, m_phoneme_b_coarse_tuning));

        AL_CALL(alEffecti(handle, AL_VOCAL_MORPHER_WAVEFORM, m_waveform));

        AL_CALL(alEffectf(handle, AL_VOCAL_MORPHER_RATE, m_rate));
    }
}
