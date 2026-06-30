#pragma once
#ifndef _TIGER_ARGS_H_
#define _TIGER_ARGS_H_
#include <vector>
#include <array>
#include <string>
#include <map>

#if defined(_WIN32) || defined(_WIN64)
    #ifdef Tiger_EXPORTS
    #define Tiger_API __declspec(dllexport)
    #else
    #define Tiger_API __declspec(dllimport)
    #endif
#else
    #ifdef Tiger_EXPORTS
    #define Tiger_API __attribute__((visibility("default")))
    #else
    #define Tiger_API
    #endif
#endif

namespace TiGER
{
}
#endif // _TIGER_ARGS_H_
