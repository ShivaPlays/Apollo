#pragma once

#include <iostream>
#include "utility/gl_loader.h"

#ifndef NDEBUG
inline const char* gl_error_string(GLenum err)
{
    switch (err)
    {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default: return "Unknown GL error";
    }
}

inline void gl_check_errors(const char* expr, const char* file, int line)
{
    if (GLenum err; (err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "[OpenGL Error] " << gl_error_string(err)
                  << " at " << file << ":" << line
                  << " for expression: " << expr << std::endl;
    }
}

// Specialization for non-void return types
template <typename Func>
auto gl_checked(Func&& func, const char* expr, const char* file, int line)
    -> typename std::enable_if<!std::is_void<decltype(func())>::value, decltype(func())>::type
{
    auto result = func();
    gl_check_errors(expr, file, line);
    return result;
}

// Specialization for void return types
template <typename Func>
auto gl_checked(Func&& func, const char* expr, const char* file, int line)
    -> typename std::enable_if<std::is_void<decltype(func())>::value, void>::type
{
    func();
    gl_check_errors(expr, file, line);
}

#define GL_CALL(expr) \
gl_checked([&]() { return (expr); }, #expr, __FILE__, __LINE__)

#else

#define GL_CALL(expr) (expr)

#endif