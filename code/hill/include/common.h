#pragma once
#ifndef _TIGER_COMMON_H_
#define _TIGER_COMMON_H_
#include <vector>
#include <array>
#include <string>
#include <map>
namespace TiGER
{
    namespace SystemMonitor
    {

        /**
         * @brief 获取当前程序的内存消耗
         *
         * @param[out] memory_usage 输出当前程序的内存消耗量，单位为字节（Bytes）。
         * @return true 获取成功
         * @return false 获取失败，可能因为系统不支持查询或其他内部错误
         */
        bool getCurrentMemoryUsage(size_t &memory_usage);

        /**
         * @brief 获取当前程序的CPU时间消耗
         *
         * @param[out] cpu_time 输出当前程序的CPU时间消耗，单位为秒（Seconds）。
         * 这包括了程序的用户态时间和内核态时间。
         * @return true 获取成功
         * @return false 获取失败，可能因为系统不支持查询或其他内部错误
         */
        bool getCurrentCPUTime(double &cpu_time);
    }
#endif // _TIGER_ARGS_H_