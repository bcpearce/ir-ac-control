#ifndef _UTIL_local_H
#define _UTIL_local_H

#include <sstream>
#include <system_error>
#include <string.h>

#define SYSTEM_WRAPPER(FUNC, ...) SystemWrapper(FUNC, #FUNC, ##__VA_ARGS__)

template<class F, class... Ts>
auto SystemWrapper(F func, const char* funcName, Ts... args) {
    auto res = func(args...);
    if(res == -1) {
        std::stringstream msg;
        msg << "Error " << errno << " from system call: " << funcName << "\n";
        msg << strerror(errno) << "\n";
        throw std::system_error(std::make_error_code(std::errc(errno)), msg.str());
    }
    return res;
}

#endif