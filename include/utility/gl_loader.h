//
// Created by skaldi on 11.02.26.
//

#pragma once

#include <SDL3/SDL_platform_defines.h>

#ifndef SDL_PLATFORM_ANDROID
#include <glad/glad.h>
#else
#include <GLES3/gl3.h>
#endif