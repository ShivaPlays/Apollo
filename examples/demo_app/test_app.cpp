#include "test_app.h"

#include <SDL3/SDL.h>

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "graphics/render_states.h"
#include "graphics/image.h"
#include "graphics/texture.h"
#include "system/assetstream.h"
#include "system/memstream.h"

#include "audio/format.h"
#include "audio/buffer.h"
#include "audio/device.h"

#include "../cmake-build-debug/_deps/openal-soft-src/include/AL/al.h"

age::engine::app_result test_app::on_init(int argc, char* argv[])
{
    static constexpr uint32_t SCREEN_WIDTH = 540;
    static constexpr uint32_t SCREEN_HEIGHT = 960;

    start("Apollo goes outer space", 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    return app_result::keep_running;
}

age::engine::app_result test_app::on_user_create()
{
    age::image test_image;
    test_image.load("./test_data/test.png");

    std::ifstream fs{ "./test_data/test.frag" };
    std::stringstream buffer;

    if (fs)
        buffer << fs.rdbuf();
   
    auto shader1 = age::shader{ age::shader::shader_type::fragment };
    shader1.compile(buffer.str());

    m_background_program.attach_shader(engine::get_instance()->get_default_vertex_shader());
    m_background_program.attach_shader(shader1);

    m_background_program.bind_attrib_location(A_POSITION_INDEX, "a_position");
    m_background_program.bind_attrib_location(A_COLOR_INDEX, "a_color");
    m_background_program.bind_attrib_location(A_TEX_COORDS_INDEX, "a_tex_coords");

    m_background_program.link();

    auto window_size = get_render_window().get_size();

    m_background_program.set_uniform("iResolution", static_cast<float>(window_size.x), static_cast<float>(window_size.y));
    m_background_program_time_location = m_background_program.get_uniform_location("iTime");

    m_background_program.set_uniform_block_binding("viewprojection_matrix", get_vp_matrix_binding());
    m_background_program.set_uniform_block_binding("model_matrix", get_model_matrix_binding());
    m_background_program.set_uniform_block_binding("texture_matrices", get_texture_matrix_binding());
    
    m_test_texture.load("./test_data/test.png");
    m_test_font_texture.load("./test_data/font_texture.png");
    m_rect_matrix = glm::translate(m_rect_matrix, glm::vec3{ 0.5f, 1.5f, 0.0f });
    
    uint32_t text_size = 64;
    m_test_font.load("./test_data/comic.ttf");
    //m_test_font.pre_cache_glyphs(U"I am a wildfire and I can save this thing into a texure also with number 0123456789 some special chars !\"�$%/()=? and don't forget about Zonk or zuzu! Yolo ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", text_size, false, 0.0f);
    //auto font_image = m_test_font.get_texture(text_size).copy_to_image();
    //font_image.save("./test_data/font_texture.png");

    m_test_text.set_character_size(64);
    m_test_text.set_font(m_test_font);
    m_test_text.set_string("Apollo flies way\nhigher\nthan the moon!");
    m_test_text.set_scale(glm::vec2{ (1.0f / static_cast<float>(window_size.x)), (1.0f / static_cast<float>(window_size.x)) });
    m_test_text.set_position(glm::vec2{ 0.0f, 0.2f });

    m_fps_text.set_character_size(64);
    m_fps_text.set_font(m_test_font);
    m_fps_text.set_string("fps: ");
    m_fps_text.set_scale(glm::vec2{ (1.0f / static_cast<float>(window_size.x)), (1.0f / static_cast<float>(window_size.x)) });
    m_fps_text.set_position(glm::vec2{ 0.0f, 0.05f });

    m_test_rectangle_shape.set_scale(glm::vec2{ 1.0f, 1.0f });
    m_test_rectangle_shape.set_position(glm::vec2{ 0.0f, 1.0f });
    m_test_rectangle_shape.set_outline_thickness(0.025f);
    m_test_rectangle_shape.set_outline_color(age::color::magenta);

    m_test_circle_shape.set_position(glm::vec2{ 0.25f, 0.25f });
    m_test_circle_shape.set_outline_thickness(0.025f);
    m_test_circle_shape.set_outline_color(age::color::magenta);
    m_test_circle_shape.set_resolution(6);

    //Testing memstream
 
    age::assetistream is{ "./test_data/laser.wav", std::ios::binary | std::ios::ate };
    auto file_size = is.tellg();
    is.seekg(0);
    std::vector<std::byte> file_data{ static_cast<size_t>(file_size) };
    is.read(reinterpret_cast<char*>(&file_data[0]), file_size);

    age::memistream ms{ &file_data[0], file_data.size() };

    m_test_buffer.load(ms);
    

    //Testing assetstream
    /*
    age::assetistream is{ "./test_data/laser.wav", std::ios::binary };
    m_test_buffer.load(is);
    */

    m_test_effect.set_gain_hf(0.1f);
    m_test_effect.set_decay_time(1.5f);
    m_test_effect.set_air_absorption_gain_hf(0.9f);
    m_test_effect.set_decay_hf_ratio(0.1f);
    m_test_effect.set_density(1.0f);
    m_test_filter.set_gain(1.0f);
    m_test_filter.set_gain_hf(0.1f);
    age::audio::device::get().get_auxiliary_send_group(1).set_enabled(true);
    age::audio::device::get().get_auxiliary_send_group(1).get_slot(0).attach_effect(&m_test_effect);
    age::audio::device::get().get_auxiliary_send_group(1).get_slot(0).attach_filter(&m_test_filter);
    age::audio::device::get().get_auxiliary_send_group(1).get_slot(0).set_volume(1.0f);

    m_test_sound.set_buffer(&m_test_buffer);

    m_test_music.open("./test_data/menu.ogg");
    m_test_music.set_volume(0.5f);
    m_test_music.set_auxiliary_bus(1);
    m_test_music.set_direct_channels(false);

    m_test_music.play(true);

    m_clock.start();

    return app_result::keep_running;
}

age::engine::app_result test_app::on_update()
{
    m_delta_time = static_cast<float>(m_clock.restart());

    m_fps_stringstream.str("");
    m_fps_stringstream.clear();
    m_fps_stringstream << "fps: " << (1.0f / m_delta_time);

    m_fps_text.set_string(m_fps_stringstream.str());

    m_elapsed_time += m_delta_time;
    m_background_program.set_uniform(m_background_program_time_location, m_elapsed_time);

    if (m_key_left)
    {
        glm::vec3 dir{ -1.0f, 0.0f, 0.0f };
        dir *= m_delta_time;
        m_rect_matrix = glm::translate(m_rect_matrix, dir);
    }

    if (m_key_right)
    {
        glm::vec3 dir{ 1.0f, 0.0f, 0.0f };
        dir *= m_delta_time;
        m_rect_matrix = glm::translate(m_rect_matrix, dir);
    }

    //Clear screen
    get_render_window().clear();

    glm::mat4 background_matrix{ 1.0f };
    background_matrix = glm::translate(background_matrix, glm::vec3{ 0.0f, 0.0f, 0.0f });
    //background_matrix.scale(age::vector2f{ 0.75f, 0.75f });
    
    //ToDo: redesign the way to render, so that the VAO does not need to be activated and released like this
    //engine::get_instance()->get_default_vertex_array_object().bind();
    //Draw Stuff
    get_render_window().draw(m_vertices.data(), m_vertices.size(), m_indizes.data(), m_indizes.size(), age::render_states{ m_background_program, m_test_texture, background_matrix });
    get_render_window().draw(m_rect_vertices.data(), m_vertices.size(), m_indizes.data(), m_indizes.size(), age::render_states{engine::get_instance()->get_default_shader_program(), m_test_texture, m_rect_matrix});
    get_render_window().draw(m_test_text, age::render_states{});
    get_render_window().draw(m_fps_text, age::render_states{});

    //get_render_window().draw(m_test_rectangle_shape, age::render_states{});
    //get_render_window().draw(m_test_circle_shape, age::render_states{});
    //get_render_window().draw(m_text_test_vertices.data(), m_text_test_vertices.size(), age::primitive_type::triangles, age::render_states{engine::get_instance()->get_default_shader_program(), m_test_font_texture, glm::scale(glm::mat4{1.0}, glm::vec3{0.01f, 0.01f, 1.0f})});

    //ToDo: with redesing the release should be removed as well
    //engine::get_instance()->get_default_vertex_array_object().release();

    //Update screen
    get_render_window().display();

    return engine::app_result::keep_running;
}

void test_app::on_user_destroy()
{

}

age::engine::app_result test_app::on_process_event(SDL_Event& e)
{
    switch (e.type)
    {
        case SDL_EVENT_QUIT:
        {
            return app_result::exit_success;
        }
        break;

        case SDL_EVENT_WINDOW_RESIZED:
        {
            std::cout << "window size changed to: " << e.window.data1 << ", " << e.window.data2 << '\n';
        }
        break;

        case SDL_EVENT_KEY_DOWN:
            switch (e.key.key)
            {
                case SDLK_LEFT:
                    m_key_left = true;
                    break;
                case SDLK_RIGHT:
                    m_key_right = true;
                    break;
            }
            break;

        case SDL_EVENT_KEY_UP:
        {
            switch (e.key.key)
            {
            case SDLK_ESCAPE:
                return app_result::exit_success;
                break;
            case SDLK_LEFT:
                m_key_left = false;
                break;
            case SDLK_RIGHT:
                m_key_right = false;
                break;
            case SDLK_S:
                m_test_sound.play();
                break;
            case SDLK_A:
                {
                    auto sound_pos = m_test_sound.get_position();
                    sound_pos.x -= 1.0f;
                    m_test_sound.update_position(sound_pos);
                }
                break;
            case SDLK_D:
                {
                    auto sound_pos = m_test_sound.get_position();
                    sound_pos.x += 1.0f;
                    m_test_sound.update_position(sound_pos);
                }
                break;
            case SDLK_P:
                {

                    switch (m_test_music.get_state())
                    {
                    case age::audio::state::playing:
                        {
                            m_test_music.pause();
                        }
                        break;
                    case age::audio::state::paused:
                        {
                            m_test_music.play(true);
                        }
                        break;
                    }

                }
                break;
            case SDLK_T:
                {
                    m_test_music.stop();
                }
                break;
            case SDLK_R:
                {
                    m_test_music.play();
                }
                break;
            case SDLK_C:
                {

                    m_test_music.open("./test_data/track1.ogg");
                    m_test_music.set_volume(1.0f);
                    m_test_music.play(true);

                }
                break;
            case SDLK_X:
                {
                    age::audio::device::get().stop_all_sounds();
                }
                break;
            case SDLK_1:
                {
                    switch (m_text_counter % 4)
                    {
                    case 0:
                        m_test_text.set_string("Hello my Name\nis Jimmy Pop");
                        break;
                    case 1:
                        m_test_text.set_string("And I'm a dump\nwhite guy");
                        break;
                    case 2:
                        m_test_text.set_string("The roof,\nthe roof");
                        break;
                    case 3:
                        m_test_text.set_string("The roof\nis on fire");
                        break;
                    default:
                        m_test_text.set_string("Ullibububuauaaddad");
                    }

                    m_text_counter++;
                }
                break;

            case SDLK_2:
                {
                    const int num_sounds = 100;
                    auto old_volume = m_test_sound.get_volume();

                    m_test_sound.set_volume(0.001f);

                    //m_test_sound.play();

                    // 1. Start the timer
                    auto start = std::chrono::high_resolution_clock::now();

                    for (size_t i = 0; i < num_sounds; ++i)
                    {
                        //alSourcePlay(2);
                        m_test_sound.play();
                    }

                    auto end = std::chrono::high_resolution_clock::now();

                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                    m_test_sound.set_volume(old_volume);

                    std::cout << "Time to play " << num_sounds << " sounds: " << duration.count() / 1000.0 << " ms" << std::endl;
                }
                break;
            }
            break;
        }
    }

    return engine::app_result::keep_running;
}