#pragma once

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include "SDL3/SDL_messagebox.h"

#include <iostream>
#include <memory>
#include <array>

#include "engine.h"

#define IMPLEMENT_APP(AppType) \
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) { \
return age::app_init<AppType>(appstate, argc, argv); \
}

namespace age
{
    template<typename AppT>
    SDL_AppResult app_init(void** appstate, int argc, char** argv)
    {
        return wrap_sdl_call([appstate, argc, argv] {
            auto app = std::make_unique<AppT>();
            auto result = engine_init(*app, argc, argv);
            *appstate = app.release();

            return to_sdl_app_result(result);
        });
    }

    template<typename Func>
    SDL_AppResult wrap_sdl_call(Func&& func)
    {
        try
        {
            return func();
        }
        catch (const std::exception& e)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unhandled exception", e.what(), nullptr);

            return SDL_APP_FAILURE;
        }
        catch (...)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unhandled exception", "Unknown error", nullptr);

            return SDL_APP_FAILURE;
        }
    }

    inline engine::app_result engine_init(age::engine& engine, int argc, char** argv)
    {
        return engine.init(argc, argv);
    }

    inline engine::app_result engine_update(age::engine& engine)
    {
        return engine.update();
    }

    inline engine::app_result engine_process_event(age::engine& engine, SDL_Event* event)
    {
        //This means we lost our context, but SDL has created a new one
        if (event->type == SDL_EVENT_RENDER_DEVICE_RESET)
        {
            engine::set_device_reset(true);
            engine::reset_render_cache();
            auto result = engine.process_event(*event);
            engine::set_device_reset(false);

            return result;
        }

        return engine.process_event(*event);
    }

    inline void engine_quit(age::engine& engine)
    {
        engine.user_destroy();
    }

    inline SDL_AppResult to_sdl_app_result(engine::app_result value)
    {
        std::array<SDL_AppResult, 3> result_arr{SDL_APP_CONTINUE, SDL_APP_SUCCESS, SDL_APP_FAILURE};

        return result_arr[static_cast<std::underlying_type_t<decltype(value)>>(value)];
    }
}


inline SDL_AppResult SDL_AppIterate(void* appstate)
{
    return age::wrap_sdl_call([appstate](){
        auto app = static_cast<age::engine*>(appstate);
        auto result = engine_update(*app);
        return to_sdl_app_result(result);
    });
}

inline SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    return age::wrap_sdl_call([appstate, event]() {

        auto app = static_cast<age::engine*>(appstate);

        auto result = engine_process_event(*app, event);
        return to_sdl_app_result(result);
     });
}

inline void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (!appstate) return;

    std::unique_ptr<age::engine> app(static_cast<age::engine*>(appstate));
    engine_quit(*app);
}