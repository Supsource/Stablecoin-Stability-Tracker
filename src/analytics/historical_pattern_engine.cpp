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

void HistoricalPatternEngine::AddPriceData(const std::string& symbol, const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& price_points) {
    auto& vec = price_history_[symbol];
    vec.insert(vec.end(), price_points.begin(), price_points.end());
    std::cout << "Added " << price_points.size() << " price points for " << symbol << "\n";
}

void HistoricalPatternEngine::AddPriceData(const std::string& symbol, const std::vector<double>& prices) {
    auto now = std::chrono::system_clock::now();
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> pts;
    for (size_t i = 0; i < prices.size(); ++i) {
        pts.emplace_back(now + std::chrono::seconds(static_cast<long>(i)), prices[i]);
    }
    AddPriceData(symbol, pts);
}

std::vector<int> HistoricalPatternEngine::DetectDepegEvents(const std::string& symbol, double target_price, double threshold) {
    std::vector<int> events;
    auto it = price_history_.find(symbol);
    if (it == price_history_.end()) return events;
    const auto& vec = it->second;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (std::abs(vec[i].second - target_price) > threshold) {
            events.push_back(static_cast<int>(i));
        }
    }
    std::cout << "Detecting depeg events for " << symbol << ": " << events.size() << " found\n";
    return events;
}

std::vector<double> windowedPrices(
    const std::map<std::string, std::vector<std::pair<std::chrono::system_clock::time_point, double>>>& history,
    const std::string& symbol,
    int historical_window_days
) {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24 * historical_window_days);
    auto it = history.find(symbol);
    if (it == history.end()) return {};
    std::vector<double> windowed_prices;
    for (const auto& [ts, price] : it->second) {
        if (ts >= cutoff) {
            windowed_prices.push_back(price);
        }
    }
    // Legacy AddPriceData uses "now"+offset; if window filter drops everything, use full series
    if (windowed_prices.empty()) {
        for (const auto& [ts, price] : it->second) {
            windowed_prices.push_back(price);
        }
    }
    return windowed_prices;
}

double HistoricalPatternEngine::CalculatePegDeviation(const std::string& symbol, double target_price) {
    auto prices = windowedPrices(price_history_, symbol, historical_window_days_);
    if (prices.empty()) return 0.0;
    return std::abs(prices.back() - target_price);
}

double HistoricalPatternEngine::CalculateVolatility(const std::string& symbol) {
    auto windowed_prices = windowedPrices(price_history_, symbol, historical_window_days_);
    std::cout << "[INFO] Using historical window of " << historical_window_days_ << " days for " << symbol << "\n";
    if (windowed_prices.size() < 2) return 0.0;
    std::vector<double> log_returns;
    log_returns.reserve(windowed_prices.size() - 1);
    for (size_t i = 1; i < windowed_prices.size(); ++i) {
        if (windowed_prices[i - 1] > 0.0 && windowed_prices[i] > 0.0) {
            log_returns.push_back(std::log(windowed_prices[i] / windowed_prices[i - 1]));
        }
    }
    if (log_returns.size() < 2) return 0.0;
    double mean = std::accumulate(log_returns.begin(), log_returns.end(), 0.0) / static_cast<double>(log_returns.size());
    double var = 0.0;
    for (auto r : log_returns) var += (r - mean) * (r - mean);
    var /= static_cast<double>(log_returns.size() - 1);
    return std::sqrt(std::max(var, 0.0));
}
}
