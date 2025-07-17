#include "../../include/core/config.hpp"
#include <iostream>

namespace stablecoin_tracker {
Config ConfigManager::LoadConfig(const std::string& path) {
    Config cfg;
    cfg.stablecoins = {"USDT", "USDC", "DAI"};
    cfg.coingecko_api = "https://api.coingecko.com";
    cfg.websocket_url = "wss://ws-feed";
    cfg.depeg_threshold = 0.02;
    cfg.risk_score_alert = 0.7;
    cfg.lookback_hours = 24;
    std::cout << "Loaded config from " << path << "\n";
    return cfg;
}
}
