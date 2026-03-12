#pragma once

#include <iostream>
#include <type_traits>
#include <AL/al.h>
#include <AL/alc.h>

#ifndef NDEBUG

// --- AL Error Mapping ---
inline const char* al_error_string(ALenum err) {
    switch (err) {
        case AL_NO_ERROR:          return "AL_NO_ERROR";
        case AL_INVALID_NAME:      return "AL_INVALID_NAME";
        case AL_INVALID_ENUM:      return "AL_INVALID_ENUM";
        case AL_INVALID_VALUE:     return "AL_INVALID_VALUE";
        case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
        case AL_OUT_OF_MEMORY:     return "AL_OUT_OF_MEMORY";
        default:                   return "Unknown AL error";
    }
}

// --- ALC Error Mapping ---
inline const char* alc_error_string(ALCenum err) {
    switch (err) {
        case ALC_NO_ERROR:        return "ALC_NO_ERROR";
        case ALC_INVALID_DEVICE:  return "ALC_INVALID_DEVICE";
        case ALC_INVALID_CONTEXT: return "ALC_INVALID_CONTEXT";
        case ALC_INVALID_ENUM:    return "ALC_INVALID_ENUM";
        case ALC_INVALID_VALUE:   return "ALC_INVALID_VALUE";
        case ALC_OUT_OF_MEMORY:   return "ALC_OUT_OF_MEMORY";
        default:                  return "Unknown ALC error";
    }
}

// --- Internal Checkers ---
inline void al_check_errors(const char* expr, const char* file, int line) {
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        std::cerr << "[OpenAL Error] " << al_error_string(err)
                  << " at " << file << ":" << line
                  << " | Expr: " << expr << std::endl;
    }
}

inline void alc_check_errors(ALCdevice* device, const char* expr, const char* file, int line) {
    ALCenum err = alcGetError(device);
    if (err != ALC_NO_ERROR) {
        std::cerr << "[ALC Error] " << alc_error_string(err)
                  << " at " << file << ":" << line
                  << " | Expr: " << expr << std::endl;
    }
}

// --- Templates ---

// Standard AL_CALL wrapper
template <typename Func>
auto al_checked(Func&& func, const char* expr, const char* file, int line) -> decltype(func()) {
    if constexpr (std::is_void_v<decltype(func())>) {
        func();
        al_check_errors(expr, file, line);
    } else {
        auto result = func();
        al_check_errors(expr, file, line);
        return result;
    }
}

// New ALC_CALL wrapper (requires device)
template <typename Func>
auto alc_checked(ALCdevice* device, Func&& func, const char* expr, const char* file, int line) -> decltype(func()) {
    if constexpr (std::is_void_v<decltype(func())>) {
        func();
        alc_check_errors(device, expr, file, line);
    } else {
        auto result = func();
        alc_check_errors(device, expr, file, line);
        return result;
    }
}

#define AL_CALL(expr) \
    al_checked([&]() { return (expr); }, #expr, __FILE__, __LINE__)

#define ALC_CALL(device, expr) \
    alc_checked(device, [&]() { return (expr); }, #expr, __FILE__, __LINE__)

#else

#define AL_CALL(expr) (expr)
#define ALC_CALL(device, expr) (expr)

#endif