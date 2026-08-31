#pragma once
#include <string>
#include <vector>
#include <chrono>

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
struct RiskWeights {
    double peg_deviation = 0.25;
    double volatility = 0.25;
    double onchain = 0.20;
    double sentiment = 0.15;
    double tvl = 0.15;
};
struct RiskAssessment {
    std::string symbol;
    double risk_score = 0.0; // 0.0 to 1.0
    double price_deviation = 0.0;
    double volume_anomaly = 0.0;
    double liquidity_risk = 0.0;
    double sentiment_risk = 0.0;
    double tvl_risk = 0.0;
    double sentiment_score = 0.0;
    std::chrono::system_clock::time_point timestamp{};
    std::string reasoning;
};
struct Alert {
    std::string symbol;
    std::string level;
    double risk_score = 0.0;
    std::string reason;
};
struct Config {
    std::vector<std::string> stablecoins;
    double target_price = 1.0;
    double depeg_threshold = 0.02;
    std::string coingecko_api;
    std::string etherscan_api;
    std::string uniswap_api;
    std::string websocket_url;
    double price_deviation_threshold = 0.02;
    double volume_spike_threshold = 0.0;
    double liquidity_drop_threshold = 0.0;
    double sentiment_threshold = 0.0;
    double risk_score_alert_threshold = 0.5;
    int max_data_points_per_sec = 0;
    int alert_latency_ms = 0;
    int historical_lookback_hours = 24;
    int analysis_window_minutes = 0;
    bool email_enabled = false;
    bool webhook_enabled = false;
    std::string webhook_url;
    std::string log_level;
    RiskWeights risk_weights;
};
struct MarketData {
    std::vector<PriceData> prices;
    std::vector<OnChainData> on_chain_events;
    std::vector<SentimentData> sentiment;
    std::chrono::system_clock::time_point last_update;
};

} 
