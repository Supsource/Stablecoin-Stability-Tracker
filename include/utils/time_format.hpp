#pragma once
#include <string>
#include <ctime>
#include <chrono>

namespace stablecoin_tracker {

inline std::tm localTimeSafe(std::time_t t) {
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

inline std::string formatTime(const char* fmt, std::time_t t = std::time(nullptr)) {
    char buf[64];
    std::tm tm = localTimeSafe(t);
    std::strftime(buf, sizeof(buf), fmt, &tm);
    return buf;
}

}
