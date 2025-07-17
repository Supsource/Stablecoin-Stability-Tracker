#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include "../third_party/json.hpp"

namespace stablecoin_tracker {
class ConfigManager {
public:
    ConfigManager(const std::string& config_path);
    bool load();
    std::vector<std::string> getStablecoins() const;
    double getThresholdFor(const std::string& stablecoin) const;
    bool isDataSourceEnabled(const std::string& source) const;
    int getHistoricalWindowDays() const;
    std::vector<std::string> getOnchainMetrics() const;
    std::vector<std::string> getMarketMetrics() const;
    std::string getEtherscanApiKey() const;
private:
    std::string config_path_;
    nlohmann::json config_json_;
    std::set<std::string> enabled_sources_;
};
} 