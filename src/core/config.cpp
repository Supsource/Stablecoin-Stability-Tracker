#include "../../include/core/config.hpp"

namespace stablecoin_tracker {
Config LoadDefaultConfig() {
    Config cfg;
    cfg.stablecoins = {"USDT", "USDC", "DAI"};
    cfg.coingecko_api = "https://api.coingecko.com/api/v3";
    cfg.websocket_url = "wss://ws-feed";
    cfg.depeg_threshold = 0.02;
    cfg.target_price = 1.0;
    cfg.risk_score_alert_threshold = 0.7;
    cfg.historical_lookback_hours = 24;
    return cfg;
}
}
