#include "../../include/core/config_manager.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace stablecoin_tracker {
ConfigManager::ConfigManager(const std::string& config_path) : config_path_(config_path) {}

bool ConfigManager::load() {
    std::ifstream in(config_path_);
    if (!in) {
        std::cerr << "Failed to open config file: " << config_path_ << std::endl;
        return false;
    }
    in >> config_json_;
    // Cache enabled sources for fast lookup
    enabled_sources_.clear();
    if (config_json_.contains("enabled_data_sources")) {
        for (const auto& src : config_json_["enabled_data_sources"]) {
            enabled_sources_.insert(src.get<std::string>());
        }
    }
    return true;
}

std::vector<std::string> ConfigManager::getStablecoins() const {
    // Access stablecoins from config
    std::vector<std::string> result;
    if (config_json_.contains("stablecoins")) {
        for (const auto& s : config_json_["stablecoins"]) {
            result.push_back(s.get<std::string>());
        }
    }
    return result;
}

double ConfigManager::getThresholdFor(const std::string& stablecoin) const {
    // Access alert threshold for a stablecoin
    if (config_json_.contains("alert_thresholds") && config_json_["alert_thresholds"].contains(stablecoin)) {
        return config_json_["alert_thresholds"][stablecoin].get<double>();
    }
    return 0.5; // Default threshold
}

bool ConfigManager::isDataSourceEnabled(const std::string& source) const {
    // Check if a data source is enabled
    return enabled_sources_.count(source) > 0;
}

int ConfigManager::getHistoricalWindowDays() const {
    // Access historical window
    if (config_json_.contains("historical_window_days")) {
        return config_json_["historical_window_days"].get<int>();
    }
    return 30;
}

std::vector<std::string> ConfigManager::getOnchainMetrics() const {
    // Access onchain metrics
    std::vector<std::string> result;
    if (config_json_.contains("onchain_metrics")) {
        for (const auto& m : config_json_["onchain_metrics"]) {
            result.push_back(m.get<std::string>());
        }
    }
    return result;
}

std::vector<std::string> ConfigManager::getMarketMetrics() const {
    // Access market metrics
    std::vector<std::string> result;
    if (config_json_.contains("market_metrics")) {
        for (const auto& m : config_json_["market_metrics"]) {
            result.push_back(m.get<std::string>());
        }
    }
    return result;
}

std::string ConfigManager::getEtherscanApiKey() const {
    std::ifstream in("config/settings.cfg");
    if (!in) {
        std::cerr << "[WARN] Could not open config/settings.cfg for Etherscan API key." << std::endl;
        return "";
    }
    std::string line;
    bool in_api_section = false;
    while (std::getline(in, line)) {
        if (line.find("[api_keys]") != std::string::npos) {
            in_api_section = true;
            continue;
        }
        if (in_api_section) {
            if (line.empty() || line[0] == '[') break;
            auto pos = line.find("etherscan_api_key");
            if (pos != std::string::npos) {
                auto eq = line.find('=', pos);
                if (eq != std::string::npos) {
                    std::string value = line.substr(eq + 1);
                    // Trim whitespace
                    value.erase(0, value.find_first_not_of(" \t\n\r"));
                    value.erase(value.find_last_not_of(" \t\n\r") + 1);
                    return value;
                }
            }
        }
    }
    std::cerr << "[WARN] etherscan_api_key not found in [api_keys] section of settings.cfg." << std::endl;
    return "";
}
} 