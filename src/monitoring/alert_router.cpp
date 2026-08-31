#include "../../include/monitoring/alert_router.h"
#include "../../include/utils/time_format.hpp"
#include <iostream>
#include <fstream>
#include <mutex>
#include <filesystem>

namespace stablecoin_tracker {
namespace {
std::mutex g_alert_log_mutex;
}

void sendAlert(const Alert& alert) {
    std::cout << "[ALERT ROUTER] Alert for " << alert.symbol << " (" << alert.level << "): routed to console\n";
    std::lock_guard<std::mutex> lock(g_alert_log_mutex);
    std::filesystem::create_directories("logs");
    std::ofstream log("logs/alerts.log", std::ios::app);
    if (log) {
        log << "[" << formatTime("%Y-%m-%d %H:%M") << "] " << alert.symbol
            << " ALERT: " << alert.level << " risk, reason: "
            << (alert.reason.empty() ? "threshold exceeded" : alert.reason) << "\n";
    }
}
}
