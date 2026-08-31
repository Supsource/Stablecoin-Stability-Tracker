#include "../../include/analytics/pattern_engine.h"
#include "../../include/analytics/pattern_matcher.hpp"
#include <iostream>
#include <chrono>
#include <ctime>

namespace stablecoin_tracker {
void detectHistoricalAnomalies(const std::vector<PricePoint>& history) {
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> prices;
    prices.reserve(history.size());
    for (const auto& p : history) {
        prices.emplace_back(std::chrono::system_clock::from_time_t(static_cast<std::time_t>(p.timestamp)), p.price);
    }
    PatternMatcher matcher(prices);
    std::string name;
    double conf = matcher.matchDepegPattern(PatternMatcher::getKnownDepegPatterns(), 0.7, &name);
    if (conf >= 0.7) {
        std::cout << "[PATTERN] Similarity to " << name << ": " << static_cast<int>(conf * 100) << "%\n";
    } else {
        std::cout << "[PATTERN] No match found in historical depeg patterns\n";
    }
}
}
