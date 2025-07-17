#pragma once
#include <string>
#include <mutex>
#include <fstream>
#include "../third_party/json.hpp"

namespace stablecoin_tracker {
class PerfLogger {
public:
    PerfLogger(const std::string& perf_csv, const std::string& alerts_json);
    void logPerformance(const std::string& coin, double fetch_time, double risk_time, double alert_time, double total_time);
    void logAlert(const nlohmann::json& alert_obj);
private:
    std::mutex mtx_;
    std::ofstream perf_csv_;
    std::ofstream alerts_json_;
    bool first_alert_ = true;
};
} 