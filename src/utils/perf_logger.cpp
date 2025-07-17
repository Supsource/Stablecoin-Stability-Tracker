#include "../../include/utils/perf_logger.hpp"
#include <iostream>

namespace stablecoin_tracker {
PerfLogger::PerfLogger(const std::string& perf_csv, const std::string& alerts_json) {
    perf_csv_.open(perf_csv, std::ios::app);
    alerts_json_.open(alerts_json, std::ios::app);
    if (perf_csv_.tellp() == 0) {
        perf_csv_ << "coin,fetch_time,risk_time,alert_time,total_time\n";
    }
    if (alerts_json_.tellp() == 0) {
        alerts_json_ << "[\n";
        first_alert_ = true;
    }
}
void PerfLogger::logPerformance(const std::string& coin, double fetch_time, double risk_time, double alert_time, double total_time) {
    std::lock_guard<std::mutex> lock(mtx_);
    perf_csv_ << coin << "," << fetch_time << "," << risk_time << "," << alert_time << "," << total_time << "\n";
    perf_csv_.flush();
}
void PerfLogger::logAlert(const nlohmann::json& alert_obj) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!first_alert_) alerts_json_ << ",\n";
    alerts_json_ << alert_obj.dump(2);
    alerts_json_.flush();
    first_alert_ = false;
}
} 