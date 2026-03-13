cmake_minimum_required(VERSION 3.25)                # Require CMake 3.25

set(LIB_NAME Apollo)
set(EXE_NAME DemoApp)

# Enable folder grouping in IDEs like Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(${LIB_NAME} VERSION 0.1 LANGUAGES C CXX)    # Project name, version, and language

set(CMAKE_CXX_STANDARD 17)                          # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)                 # Enforce that standard

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
            -Wall
            -Wextra
            -Wuninitialized
            -Wno-error=changes-meaning
    )
elseif(MSVC)
    add_compile_options(
            /W4        # Enable warning level 4
            /wd4244    # Disable: 'conversion' from 'double' to 'float', possible loss of data
            /wd4267    # Disable: 'conversion' from 'size_t' to 'int', possible loss of data
            /wd4996    # Disable: POSIX name/unsafe function warnings (common in Ogg/Vorbis)
    )
    # Ensure /WX is not added back by any property
    add_compile_options(/WX-)
endif()

# We do this early so the targets (like SDL3::SDL3) exist for later checks.
include(cmake/dependencies.cmake)

# Add your library target
add_library(${LIB_NAME} STATIC
        # Audio
        include/audio/buffer.h src/audio/buffer.cpp
        include/audio/channel.h src/audio/channel.cpp
        include/audio/channel_guard.h
        include/audio/config.h
        include/audio/device.h src/audio/device.cpp
        include/audio/format.h src/audio/format.cpp
        include/audio/listener.h src/audio/listener.cpp
        include/audio/music.h src/audio/music.cpp
        include/audio/properties.h
        include/audio/queue_buffer.h src/audio/queue_buffer.cpp
        include/audio/resource.h src/audio/resource.cpp
        include/audio/sound.h src/audio/sound.cpp
        include/audio/sound_interface.h src/audio/sound_interface.cpp
        include/audio/source.h src/audio/source.cpp
        include/audio/state.h
        include/audio/stream.h src/audio/stream.cpp
        include/audio/stream_factory.h src/audio/stream_factory.cpp
        include/audio/wave.h src/audio/wave.cpp

        # Audio Effects
        include/audio/effect/autowah.h src/audio/effect/autowah.cpp
        include/audio/effect/auxiliary_send_group.h src/audio/effect/auxiliary_send_group.cpp
        include/audio/effect/chorus.h src/audio/effect/chorus.cpp
        include/audio/effect/compressor.h src/audio/effect/compressor.cpp
        include/audio/effect/distortion.h src/audio/effect/distortion.cpp
        include/audio/effect/echo.h src/audio/effect/echo.cpp
        include/audio/effect/effect_interface.h src/audio/effect/effect_interface.cpp
        include/audio/effect/equalizer.h src/audio/effect/equalizer.cpp
        include/audio/effect/flanger.h src/audio/effect/flanger.cpp
        include/audio/effect/frequency_shifter.h src/audio/effect/frequency_shifter.cpp
        include/audio/effect/pitch_shifter.h src/audio/effect/pitch_shifter.cpp
        include/audio/effect/reverb.h src/audio/effect/reverb.cpp
        include/audio/effect/ring_modulator.h src/audio/effect/ring_modulator.cpp
        include/audio/effect/slot.h src/audio/effect/slot.cpp
        include/audio/effect/vocal_morpher.h src/audio/effect/vocal_morpher.cpp

        # Audio Filters
        include/audio/filter/band_pass.h src/audio/filter/band_pass.cpp
        include/audio/filter/filter_interface.h src/audio/filter/filter_interface.cpp
        include/audio/filter/high_pass.h src/audio/filter/high_pass.cpp
        include/audio/filter/low_pass.h src/audio/filter/low_pass.cpp

        # Audio Private
        include/audio/priv/al_check.h
        include/audio/priv/ogg_stream.h src/audio/priv/ogg_stream.cpp

        # Graphics
        include/graphics/angle.h
        include/graphics/blend_mode.h src/graphics/blend_mode.cpp
        include/graphics/circle_shape.h src/graphics/circle_shape.cpp
        include/graphics/color.h src/graphics/color.cpp
        include/graphics/context.h src/graphics/context.cpp
        include/graphics/drawable.h
        include/graphics/font.h src/graphics/font.cpp
        include/graphics/frame_buffer.h src/graphics/frame_buffer.cpp
        include/graphics/image.h src/graphics/image.cpp
        include/graphics/rect.h
        include/graphics/rectangle_shape.h src/graphics/rectangle_shape.cpp
        include/graphics/render_states.h src/graphics/render_states.cpp
        include/graphics/render_states_cache.h
        include/graphics/render_target.h src/graphics/render_target.cpp
        include/graphics/render_window.h src/graphics/render_window.cpp
        include/graphics/shader.h src/graphics/shader.cpp
        include/graphics/shader_program.h src/graphics/shader_program.cpp
        include/graphics/shape_2d.h src/graphics/shape_2d.cpp
        include/graphics/sprite.h src/graphics/sprite.cpp
        include/graphics/text.h src/graphics/text.cpp
        include/graphics/texture.h src/graphics/texture.cpp
        include/graphics/transformable_2d.h src/graphics/transformable_2d.cpp
        include/graphics/uniform_buffer_object.h src/graphics/uniform_buffer_object.cpp
        include/graphics/vertex_2d.h
        include/graphics/vertex_array_object.h src/graphics/vertex_array_object.cpp
        include/graphics/vertex_buffer_object.h src/graphics/vertex_buffer_object.cpp
        include/graphics/view_2d.h src/graphics/view_2d.cpp
        include/graphics/view_3d.h src/graphics/view_3d.cpp

        # Core & System
        include/core/background_worker.h src/core/background_worker.cpp
        include/core/dependency_tracker.h
        include/system/assetstream.h src/system/assetstream.cpp
        include/system/clock.h src/system/clock.cpp
        include/system/memstream.h src/system/memstream.cpp
        include/system/platform.h
        include/system/time.h
        include/system/transient_context_lock.h src/system/transient_context_lock.cpp

        # Utility
        include/utility/endian.h
        include/utility/fixed_string.h
        include/utility/gl_check.h
        include/utility/gl_loader.h
        include/utility/make_array.h
        include/utility/utility.h src/utility/utility.cpp

        # Root
        include/apollo_main.h
        include/engine.h src/engine.cpp
)

if(NOT ANDROID)
    target_sources(${LIB_NAME} PRIVATE extlibs/libs/glad/src/glad.c)
endif()

# Also set explicitly for project
set_target_properties(${LIB_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# Headers go here
target_include_directories(${LIB_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/headers/stb_image
        ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/headers/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/headers/gch
        ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/libs/glad/include
        $<INSTALL_INTERFACE:include>
)

# Link other libraries
target_link_libraries(${LIB_NAME}
        PRIVATE
        ${OpenGL_GL_LIBRARY}
        ${FREETYPE_TARGET}
        ${OGG_TARGET}
        ${VORBIS_TARGET}
        ${VORBISFILE_TARGET}
        ${OPENAL_TARGET}
        PUBLIC
        ${SDL3_TARGET}
        #SDL3::SDL3
)

# Android-specific linking
if(ANDROID)
    target_link_libraries(${LIB_NAME} PRIVATE android log)
else()
    target_link_libraries(${LIB_NAME} PRIVATE ${GLAD_TARGET})
endif()

# Create the DemoApp
if(NOT ANDROID)
    # Create the DemoApp only for Desktop
    add_executable(${EXE_NAME}
            examples/demo_app/main.cpp
            examples/demo_app/test_app.cpp
    )

    # Standard Threading
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    # Link everything to the App
    target_link_libraries(${EXE_NAME}
            PRIVATE
            ${LIB_NAME}
            ${SDL3_TARGET}
            Threads::Threads
    )

    # Visual Studio Specifics
    if(MSVC)
        set_target_properties(${EXE_NAME} PROPERTIES
                VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endif()

    # Windows DLL Copying
    if (WIN32)
        add_custom_command(TARGET ${EXE_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:${EXE_NAME}>
                $<TARGET_FILE_DIR:${EXE_NAME}>
                COMMAND_EXPAND_LISTS
        )
    endif()
endif()