#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace stablecoin_tracker {
struct PriceData {
    std::string symbol;
    double price;
    double volume;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
};
struct OnChainData {
    std::string token_address;
    std::string tx_hash;
    double amount;
    std::string from_address;
    std::string to_address;
    std::chrono::system_clock::time_point timestamp;
    std::string event_type; // "transfer", "mint", "burn", "swap"
};
struct SentimentData {
    std::string symbol;
    double sentiment_score; // -1.0 to 1.0
    int mention_count;
    std::string source;
    std::chrono::system_clock::time_point timestamp;
};
struct RiskAssessment {
    std::string symbol;
    double risk_score; // 0.0 to 1.0
    double price_deviation;
    double volume_anomaly;
    double liquidity_risk;
    double sentiment_risk;
    std::chrono::system_clock::time_point timestamp;
    std::string reasoning;
};
struct Alert {
    std::string symbol;
    std::string alert_type; // "DEPEG_WARNING", "DEPEG_CRITICAL", "VOLUME_SPIKE"
    double severity; // 0.0 to 1.0
    std::string message;
    RiskAssessment risk_data;
    std::chrono::system_clock::time_point timestamp;
};
struct Config {
    std::vector<std::string> stablecoins;
    double target_price;
    double depeg_threshold;
    std::string coingecko_api;
    std::string etherscan_api;
    std::string uniswap_api;
    std::string websocket_url;
    double price_deviation_threshold;
    double volume_spike_threshold;
    double liquidity_drop_threshold;
    double sentiment_threshold;
    double risk_score_alert_threshold;
    int max_data_points_per_sec;
    int alert_latency_ms;
    int historical_lookback_hours;
    int analysis_window_minutes;
    bool email_enabled;
    bool webhook_enabled;
    std::string webhook_url;
    std::string log_level;
};
struct MarketData {
    std::vector<PriceData> prices;
    std::vector<OnChainData> on_chain_events;
    std::vector<SentimentData> sentiment;
    std::chrono::system_clock::time_point last_update;
};

} 
