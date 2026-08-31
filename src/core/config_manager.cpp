#include "../../include/core/config_manager.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>

namespace stablecoin_tracker {
namespace {
std::string trim(std::string value) {
    auto start = value.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = value.find_last_not_of(" \t\n\r");
    return value.substr(start, end - start + 1);
}

std::string readKeyFromCfg(const std::string& path) {
    std::ifstream in(path);
    if (!in) return "";
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
                    return trim(line.substr(eq + 1));
                }
            }
        }
    }
    return "";
}
}

ConfigManager::ConfigManager(const std::string& config_path) : config_path_(config_path) {}

bool ConfigManager::ingestLoadedJson() {
    enabled_sources_.clear();
    if (config_json_.contains("enabled_data_sources")) {
        for (const auto& src : config_json_["enabled_data_sources"]) {
            enabled_sources_.insert(src.get<std::string>());
        }
    }
    auto coins = getStablecoins();
    if (coins.empty()) {
        std::cerr << "[ERROR] Config has no stablecoins list.\n";
        return false;
    }
    if (getHistoricalWindowDays() <= 0) {
        std::cerr << "[ERROR] historical_window_days must be positive.\n";
        return false;
    }
    return true;
}

bool ConfigManager::load() {
    auto try_path = [&](const std::string& path) -> bool {
        std::ifstream in(path);
        if (!in) return false;
        try {
            in >> config_json_;
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse config JSON (" << path << "): " << e.what() << std::endl;
            return false;
        }
        if (!ingestLoadedJson()) return false;
        std::cout << "[INFO] Loaded config from " << path << "\n";
        return true;
    };
    if (try_path(config_path_)) return true;
    if (config_path_ == "config/config.json" && try_path("config/config.json.example")) {
        std::cerr << "[WARN] Using example config. Copy it to config/config.json to customize.\n";
        return true;
    }
    std::cerr << "Failed to open config file: " << config_path_ << std::endl;
    return false;
}

std::vector<std::string> ConfigManager::getStablecoins() const {
    std::vector<std::string> result;
    if (config_json_.contains("stablecoins")) {
        for (const auto& s : config_json_["stablecoins"]) {
            result.push_back(s.get<std::string>());
        }
    }
    return result;
}

double ConfigManager::getThresholdFor(const std::string& stablecoin) const {
    if (config_json_.contains("alert_thresholds") && config_json_["alert_thresholds"].contains(stablecoin)) {
        return config_json_["alert_thresholds"][stablecoin].get<double>();
    }
    return 0.5;
}

bool ConfigManager::isDataSourceEnabled(const std::string& source) const {
    if (enabled_sources_.empty()) return true;
    return enabled_sources_.count(source) > 0;
}

int ConfigManager::getHistoricalWindowDays() const {
    if (config_json_.contains("historical_window_days")) {
        return config_json_["historical_window_days"].get<int>();
    }
    return 30;
}

std::vector<std::string> ConfigManager::getOnchainMetrics() const {
    std::vector<std::string> result;
    if (config_json_.contains("onchain_metrics")) {
        for (const auto& m : config_json_["onchain_metrics"]) {
            result.push_back(m.get<std::string>());
        }
    }
    return result;
}

std::vector<std::string> ConfigManager::getMarketMetrics() const {
    std::vector<std::string> result;
    if (config_json_.contains("market_metrics")) {
        for (const auto& m : config_json_["market_metrics"]) {
            result.push_back(m.get<std::string>());
        }
    }
    return result;
}

std::string ConfigManager::getEtherscanApiKey() const {
    if (const char* env = std::getenv("ETHERSCAN_API_KEY")) {
        std::string from_env = trim(env);
        if (!from_env.empty()) return from_env;
    }
    if (config_json_.contains("etherscan_api_key") && config_json_["etherscan_api_key"].is_string()) {
        std::string from_json = trim(config_json_["etherscan_api_key"].get<std::string>());
        if (!from_json.empty() && from_json != "YOUR_KEY") return from_json;
    }
    for (const char* path : {"config/settings.cfg", "config/settings.cfg.example"}) {
        std::string key = readKeyFromCfg(path);
        if (!key.empty() && key != "YOUR_KEY") return key;
    }
    std::cerr << "[WARN] etherscan_api_key not found (settings.cfg, config.json, or ETHERSCAN_API_KEY).\n";
    return "";
}

double ConfigManager::getTargetPrice() const {
    if (config_json_.contains("target_price")) {
        return config_json_["target_price"].get<double>();
    }
    return 1.0;
}

double ConfigManager::getDepegThreshold() const {
    if (config_json_.contains("depeg_threshold")) {
        return config_json_["depeg_threshold"].get<double>();
    }
    return 0.02;
}

RiskWeights ConfigManager::getRiskWeights() const {
    RiskWeights w;
    if (!config_json_.contains("risk_weights")) return w;
    const auto& j = config_json_["risk_weights"];
    if (j.contains("peg_deviation")) w.peg_deviation = j["peg_deviation"].get<double>();
    if (j.contains("volatility")) w.volatility = j["volatility"].get<double>();
    if (j.contains("onchain")) w.onchain = j["onchain"].get<double>();
    if (j.contains("sentiment")) w.sentiment = j["sentiment"].get<double>();
    if (j.contains("tvl")) w.tvl = j["tvl"].get<double>();
    return w;
}
}
