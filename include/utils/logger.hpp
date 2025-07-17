#pragma once
#include <string>
#include <iostream>

namespace stablecoin_tracker {
class Logger {
public:
    static void Info(const std::string& msg) {
        std::cout << "[INFO] " << msg << std::endl;
    }
    static void Debug(const std::string& msg) {
        std::cout << "[DEBUG] " << msg << std::endl;
    }
};
}
