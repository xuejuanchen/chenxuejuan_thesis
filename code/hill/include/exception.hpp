#pragma once
#ifndef _TIGER_EXCEPTION_H_
#define _TIGER_EXCEPTION_H_
#include <exception>
class CustomException : public std::runtime_error {
public:
    // 构造函数，接受一个字符串参数作为异常信息
    CustomException(const std::string& message)
        : std::runtime_error(message) {
    }
    
};

#endif //!_TIGER_EXCEPTION_H_