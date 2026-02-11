include(FetchContent)

# Helper function to fetch content without polluting the IDE
# ------------------------------
function(fetch_hide_from_ide name)
    set(options)
    set(oneValueArgs GIT_REPOSITORY GIT_TAG)
    set(multiValueArgs CMAKE_ARGS)
    cmake_parse_arguments(FH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${FH_GIT_REPOSITORY}
            GIT_TAG ${FH_GIT_TAG}
            SYSTEM
            CMAKE_ARGS ${FH_CMAKE_ARGS}
    )

    FetchContent_GetProperties(${name})
    if(NOT ${name}_POPULATED)
        # Store what the folder was before we started
        set(PREV_FOLDER ${CMAKE_FOLDER})

        # Group this specific dependency
        set(CMAKE_FOLDER "External/${name}")

        FetchContent_MakeAvailable(${name})

        # Restore the folder to exactly what it was before this function ran
        set(CMAKE_FOLDER "${PREV_FOLDER}" PARENT_SCOPE)

        if(TARGET ${name})
            set_target_properties(${name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
            set_property(TARGET ${name} PROPERTY FOLDER "External/${name}")
        endif()
    endif()
endfunction()
# ------------------------------

# ------------------------------
# OpenGL
# ------------------------------
if(NOT ANDROID)
    # Resolve 'FindOpenGL' ambiguity warning on Linux.
    # CMP0072 policy set to 'NEW' tells CMake to prefer the modern
    # GLVND (Vendor-Neutral Dispatch) libraries over the legacy 'libGL.so'.

    cmake_policy(SET CMP0072 NEW)
    find_package(OpenGL REQUIRED)
    set(OpenGL_GL_LIBRARY OpenGL::GL)
else()
    # On Android, we link GLES and EGL directly later
    set(OpenGL_GL_LIBRARY GLESv3 EGL)
    message(STATUS "Android detected: Setting OpenGL_GL_LIBRARY to GLESv3")
endif()


# HarfBuzz (Required for FreeType to pass its dependency check)
# ------------------------------
find_package(HarfBuzz QUIET)
if(NOT HarfBuzz_FOUND)
    message(STATUS "HarfBuzz not found, fetching with FetchContent...")

    fetch_hide_from_ide(
            harfbuzz
            GIT_REPOSITORY https://github.com/harfbuzz/harfbuzz.git
            GIT_TAG 12.1.0 # A recent stable tag
            CMAKE_ARGS
            # HarfBuzz requires several dependencies; disable unnecessary ones for a cleaner build
            -DHB_BUILD_TESTS=OFF
            -DHB_BUILD_TOOLS=OFF
            -DHB_HAVE_GLIB=OFF
            -DHB_HAVE_GOBJECT=OFF
            -DHB_HAVE_ICU=OFF
    )

    set(HARFBUZZ_TARGET harfbuzz)
else()
    message(STATUS "Using system-installed HarfBuzz")
    set(HARFBUZZ_TARGET HarfBuzz::HarfBuzz)
endif()

# ------------------------------
# FreeType
# ------------------------------
find_package(Freetype QUIET)
if(NOT Freetype_FOUND)
    message(STATUS "FreeType not found, fetching with FetchContent...")

    fetch_hide_from_ide(
        freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
        GIT_TAG VER-2-14-1
        CMAKE_ARGS
            # 1. Explicitly enable HarfBuzz support
            -DFT_WITH_HARFBUZZ=ON
    )

    set(FREETYPE_TARGET freetype)
else()
    message(STATUS "Using system-installed FreeType")
    set(FREETYPE_TARGET Freetype::Freetype)
endif()

# ------------------------------
# OGG (libogg)
# ------------------------------
find_package(Ogg QUIET)
if(NOT Ogg_FOUND)
    message(STATUS "Ogg not found, fetching with FetchContent...")

    fetch_hide_from_ide(
            ogg
            GIT_REPOSITORY https://github.com/xiph/ogg.git
            GIT_TAG v1.3.6  # latest stable tag at the moment
            CMAKE_ARGS
                -DCMAKE_POLICY_VERSION=3.5
    )

    set(OGG_TARGET ogg)
    set(OGG_INCLUDE_DIR ${ogg_SOURCE_DIR}/include)
    set(OGG_LIBRARY ${ogg_BINARY_DIR}/src/.libs/libogg.a)
else()
    message(STATUS "Using system-installed Ogg")
    set(OGG_TARGET Ogg::ogg)

    set(OGG_INCLUDE_DIR ${Ogg_INCLUDE_DIR})
    set(OGG_LIBRARY ${Ogg_LIBRARY})
endif()

# ------------------------------
# Vorbis (libvorbis)
# ------------------------------
find_package(Vorbis QUIET)
if(NOT Vorbis_FOUND)
    message(STATUS "Vorbis not found, fetching with FetchContent...")

    fetch_hide_from_ide(
            vorbis
            GIT_REPOSITORY https://github.com/xiph/vorbis.git
            GIT_TAG 43bbff0141028e58d476c1d5fd45dd5573db576d  # latest stable tag at the moment
            CMAKE_ARGS
            -DOGG_INCLUDE_DIR=${OGG_INCLUDE_DIR}
            -DOGG_LIBRARY=${OGG_LIBRARY}
    )

    set(VORBIS_TARGET vorbis)
    set(VORBISFILE_TARGET vorbisfile)
else()
    message(STATUS "Using system-installed Vorbis")
    set(VORBIS_TARGET Vorbis::vorbis)
    set(VORBISFILE_TARGET Vorbis::vorbisfile)
endif()

# ------------------------------
# OpenAL-Soft
# ------------------------------
find_package(OpenAL QUIET)
if(NOT OpenAL_FOUND)
    message(STATUS "OpenAL not found, fetching with FetchContent...")

    fetch_hide_from_ide(
            openal-soft
            GIT_REPOSITORY https://github.com/kcat/openal-soft.git
            #GIT_TAG 1.24.3  #This version has a bug with CMake. As soon as there is a newer one, the newer one can be used
            GIT_TAG master
    )

    set(OPENAL_TARGET OpenAL::OpenAL) # Still use the alias for linking
else()
    message(STATUS "Using system-installed OpenAL")
    set(OPENAL_TARGET OpenAL::OpenAL)

    # *** Fix for System ALIAS Target ***
    if(TARGET OpenAL::OpenAL)
        # We cannot set CXX_STANDARD on an ALIAS, but we can set features on the interface.
        message(STATUS "Fixing system OpenAL::OpenAL interface features.")
        set_target_properties(OpenAL::OpenAL PROPERTIES
                INTERFACE_CXX_FEATURES "cxx_std_17"
        )
    endif()
endif()

# ------------------------------
# SDL3
# ------------------------------
find_package(SDL3 QUIET)
if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, fetching with FetchContent...")

    # Define your platform-specific flags
    if(ANDROID)
        set(SDL_SHARED ON)
        set(SDL_STATIC OFF)
    else()
        set(SDL_SHARED OFF)
        set(SDL_STATIC ON)
    endif()

    fetch_hide_from_ide(
            sdl3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.4.0  # latest 3.4.0 patch version
    )

    set(SDL3_TARGET SDL3::SDL3)

else()
    message(STATUS "Using system-installed SDL3")
    set(SDL3_TARGET SDL3::SDL3)
endif()
