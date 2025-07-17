#include "../../include/monitoring/alert_router.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <filesystem>

namespace stablecoin_tracker {
void sendAlert(const Alert& alert) {
    std::cout << "[ALERT ROUTER] Alert for " << alert.symbol << " (" << alert.level << "): routed to console\n";
    // Write to log file
    std::filesystem::create_directories("logs");
    std::ofstream log("logs/alerts.log", std::ios::app);
    if (log) {
        std::time_t t = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&t));
        log << "[" << buf << "] " << alert.symbol << " ALERT: " << alert.level << " risk, reason: high on-chain flow\n";
    }
}
} 