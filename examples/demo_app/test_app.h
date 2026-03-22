#pragma once

#include "engine.h"

#include <sstream>

#include <glm/mat4x4.hpp>

#include "graphics/shader_program.h"
#include "graphics/vertex_2d.h"
#include "graphics/texture.h"
#include "graphics/view_2d.h"
#include "graphics/font.h"
#include "graphics/text.h"
#include "graphics/rectangle_shape.h"
#include "graphics/circle_shape.h"
#include "system/clock.h"
#include "audio/buffer.h"
#include "audio/sound.h"
#include "audio/music.h"
#include "audio/effect/eax_reverb.h"

class test_app
	: public age::engine
{
public:

public:

protected:
	

private:
    app_result on_init(int argc, char* argv[]) override;
    app_result on_user_create() override;
    engine::app_result on_update() override;
    void on_user_destroy() override;
    engine::app_result on_process_event(SDL_Event& e) override;

    static constexpr uint32_t A_POSITION_INDEX = 0;
    static constexpr uint32_t A_COLOR_INDEX = 1;
    static constexpr uint32_t A_TEX_COORDS_INDEX = 2;

    std::array<age::vertex_2d, 4> m_vertices =
    {
        age::vertex_2d{ glm::vec2{ 0.0f, 0.0f } },
        age::vertex_2d{ glm::vec2{ 1.0f, 0.0f } },
        age::vertex_2d{ glm::vec2{ 1.0f, 1.777777f } },
        age::vertex_2d{ glm::vec2{ 0.0f, 1.777777f } }
    };
    
    std::array<age::vertex_2d, 4> m_rect_vertices =
    {
        age::vertex_2d{ glm::vec2{ 0.0f, 0.0f }, age::color{255, 255, 255, 255 }, glm::vec2{0.0f, 109.0f} },
        age::vertex_2d{ glm::vec2{ 0.18f, 0.0f }, age::color{255, 255, 255, 255 }, glm::vec2{110.0f, 109.0f} },
        age::vertex_2d{ glm::vec2{ 0.18f, 0.045f }, age::color{255, 255, 255, 255 }, glm::vec2{110.0f, 136.0f} },
        age::vertex_2d{ glm::vec2{ 0.0f, 0.045f }, age::color{255, 255, 255, 255 }, glm::vec2{0.0f, 136.0f} }
    };

    std::array<uint32_t, 6> m_indizes =
    {
        0, 1, 2,
        2, 3, 0
    };

    std::array<age::vertex_2d, 6> m_text_test_vertices =
    {
        age::vertex_2d{ glm::vec2{ 3.0f, 17.0f }, age::color{255, 255, 255, 255 }, glm::vec2{275.0f, 204.0f} },
        age::vertex_2d{ glm::vec2{ 43.0f, 17.0f }, age::color{255, 255, 255, 255 }, glm::vec2{313.0f, 204.0f} },
        age::vertex_2d{ glm::vec2{ 3.0f, 66.0f }, age::color{255, 255, 255, 255 }, glm::vec2{275.0f, 251.0f} },
        age::vertex_2d{ glm::vec2{ 3.0f, 66.0f }, age::color{255, 255, 255, 255 }, glm::vec2{275.0f, 251.0f} },
        age::vertex_2d{ glm::vec2{ 43.0f, 17.0f }, age::color{255, 255, 255, 255 }, glm::vec2{313.0f, 204.0f} },
        age::vertex_2d{ glm::vec2{ 43.0f, 66.0f }, age::color{255, 255, 255, 255 }, glm::vec2{313.0f, 251.0f} }
    };

    glm::mat4 m_rect_matrix{ 1.0f };

    age::shader_program m_background_program;
    age::texture m_test_texture;
    age::texture m_test_font_texture;

    int32_t m_background_program_time_location = -1;

    age::clock m_clock;

    age::font m_test_font;
    age::text m_test_text;
    age::text m_fps_text;

    age::rectangle_shape m_test_rectangle_shape{ glm::vec2{0.25f, 0.25f} };
    age::circle_shape m_test_circle_shape{ 0.2f, 3 };

    std::stringstream m_fps_stringstream;
    std::u32string m_fps_string;

    age::audio::buffer m_test_buffer;
    age::audio::sound m_test_sound;

    age::audio::music m_test_music;

    age::audio::effect::eax_reverb m_test_effect;

    size_t m_text_counter = 0;

    float m_delta_time = 0.0;
    float m_elapsed_time = 0.0f;

    bool m_key_left = false;
    bool m_key_right = false;
};