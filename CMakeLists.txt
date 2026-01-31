cmake_minimum_required(VERSION 3.10...3.16)                # Require CMake 3.10 to 3.16

set(LIB_NAME Apollo)
set(EXE_NAME DemoApp)

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
            /W4   # Enable warning level 4 on MSVC
            /WX   # Treat warnings as errors
    )
endif()

# Add your library target
add_library(${LIB_NAME} STATIC
    src/audio/priv/ogg_stream.cpp
    src/audio/audio_device.cpp
    src/audio/audio_format.cpp
    src/audio/audio_resource.cpp
    src/audio/listener.cpp
    src/audio/music.cpp
    src/audio/sound.cpp
    src/audio/sound_buffer.cpp
    src/audio/sound_file_wave.cpp
    src/audio/sound_interface.cpp
    src/audio/sound_queue_buffer.cpp
    src/audio/sound_source.cpp
    src/audio/sound_stream.cpp
    src/audio/sound_stream_factory.cpp
    src/graphics/blend_mode.cpp
    src/graphics/circle_shape.cpp
    src/graphics/color.cpp
    src/graphics/context.cpp
    src/graphics/font.cpp
    src/graphics/image.cpp
    src/graphics/rectangle_shape.cpp
    src/graphics/render_states.cpp
    src/graphics/render_target.cpp
    src/graphics/render_window.cpp
    src/graphics/shader.cpp
    src/graphics/shader_program.cpp
    src/graphics/shape_2d.cpp
    src/graphics/sprite.cpp
    src/graphics/text.cpp
    src/graphics/texture.cpp
    src/graphics/transformable_2d.cpp
    src/graphics/uniform_buffer_object.cpp
    src/graphics/vertex_array_object.cpp
    src/graphics/vertex_buffer_object.cpp
    src/graphics/view_2d.cpp
    src/graphics/view_3d.cpp
    src/system/assetstream.cpp
    src/system/background_worker.cpp
    src/system/clock.cpp
    src/system/memstream.cpp
    src/system/transient_context_lock.cpp
    src/utility/utility.cpp
    src/engine.cpp
    extlibs/libs/glad/src/glad.c
)

# Also set explictily for project
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
        ${CMAKE_CURRENT_SOURCE_DIR}/extlibs/libs/glad/include
        $<INSTALL_INTERFACE:include>
)

# Link other libraries (e.g. freetype, SDL2).
# Dependencies are handled in file cmake/dependencies.cmake
include(cmake/dependencies.cmake)
target_link_libraries(${LIB_NAME}
        PRIVATE
            ${OpenGL_GL_LIBRARY}
            ${FREETYPE_TARGET}
            ${GLAD_TARGET}
            ${OGG_TARGET}
            ${VORBIS_TARGET}
            ${VORBISFILE_TARGET}
            ${OPENAL_TARGET}
        PUBLIC
            ${SDL3_TARGET}
)

# Also create the DemoApp
add_executable(${EXE_NAME}
        examples/demo_app/main.cpp
        examples/demo_app/test_app.cpp
)

set_target_properties(${EXE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(WIN32)
    # On Windows, enable static linking options (needed for static runtime libs)
    target_link_options(${EXE_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
else()
    # On non-Windows (e.g. Linux), do not use static linking to avoid linker errors
    message(STATUS "Skipping static linking options on non-Windows platform")
endif()

target_link_libraries(${EXE_NAME}
        PRIVATE
        ${LIB_NAME}
        ${SDL3_TARGET}
        Threads::Threads
)
target_include_directories(${EXE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# --- Copy OpenAL DLL after build (Windows only)
if (WIN32)
    # Figure out where the OpenAL DLL ended up
    # get_target_property(_openal_location OpenAL::OpenAL LOCATION)

    add_custom_command(TARGET ${EXE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:OpenAL::OpenAL>
            $<TARGET_FILE_DIR:${EXE_NAME}>
            COMMENT "Copying OpenAL DLL to output directory"
    )
endif()