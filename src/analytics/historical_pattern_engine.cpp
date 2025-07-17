#include "../../include/analytics/historical_pattern_engine.hpp"
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
#include <utility>
#include <map>
#include <numeric>

namespace stablecoin_tracker {
HistoricalPatternEngine::HistoricalPatternEngine(int lookback_hours)
    : historical_window_days_(lookback_hours / 24) {}

void HistoricalPatternEngine::SetHistoricalWindowDays(int days) {
    historical_window_days_ = days;
}

// Add price data with timestamps
void HistoricalPatternEngine::AddPriceData(const std::string& symbol, const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& price_points) {
    auto& vec = price_history_[symbol];
    vec.insert(vec.end(), price_points.begin(), price_points.end());
    std::cout << "Added " << price_points.size() << " price points for " << symbol << "\n";
}

// Legacy interface: add prices with 'now' as timestamp
void HistoricalPatternEngine::AddPriceData(const std::string& symbol, const std::vector<double>& prices) {
    auto now = std::chrono::system_clock::now();
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> pts;
    for (auto p : prices) {
        pts.emplace_back(now, p);
    }
    AddPriceData(symbol, pts);
}

std::vector<int> HistoricalPatternEngine::DetectDepegEvents(const std::string& symbol, double target_price, double threshold) {
    std::cout << "Detecting depeg events for " << symbol << "\n";
    return {1};
}

double HistoricalPatternEngine::CalculateVolatility(const std::string& symbol) {
    // Filter price 
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24 * historical_window_days_);
    const auto& vec = price_history_[symbol];
    std::vector<double> windowed_prices;
    for (const auto& [ts, price] : vec) {
        if (ts >= cutoff) {
            windowed_prices.push_back(price);
        }
    }
    std::cout << "[INFO] Using historical window of " << historical_window_days_ << " days for " << symbol << "\n";
    if (windowed_prices.size() < 2) return 0.0;
    double mean = std::accumulate(windowed_prices.begin(), windowed_prices.end(), 0.0) / windowed_prices.size();
    double var = 0.0;
    for (auto v : windowed_prices) var += (v - mean) * (v - mean);
    var /= windowed_prices.size();
    return std::sqrt(var);
}
}

