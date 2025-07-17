#pragma once
#include <string>
#include <vector>

namespace stablecoin_tracker {
struct Config {
    std::vector<std::string> stablecoins;
    std::string coingecko_api;
    std::string websocket_url;
    double depeg_threshold;
    double risk_score_alert;
    int lookback_hours;
};

class ConfigManager {
public:
    static Config LoadConfig(const std::string& path);
};
}
