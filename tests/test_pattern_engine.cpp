#include "../include/analytics/pattern_engine.h"
#include "../include/analytics/pattern_matcher.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

using namespace stablecoin_tracker;

int main() {
    std::vector<PricePoint> history = {
        {1620000000, 1.00}, {1620003600, 0.99}, {1620007200, 0.98}, {1620010800, 0.97}
    };
    detectHistoricalAnomalies(history);

    auto now = std::chrono::system_clock::now();
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> collapse;
    std::vector<double> ust = {1.0, 0.98, 0.92, 0.85, 0.7, 0.2};
    for (size_t i = 0; i < ust.size(); ++i) {
        collapse.emplace_back(now + std::chrono::seconds(static_cast<long>(i)), ust[i]);
    }
    PatternMatcher matcher(collapse);
    std::string name;
    double conf = matcher.matchDepegPattern(PatternMatcher::getKnownDepegPatterns(), 0.7, &name);
    if (conf < 0.7) {
        std::cout << "[FAIL] Expected UST-like series to match a known pattern, got " << conf << "\n";
        return 1;
    }
    std::cout << "[PASS] Pattern engine matched " << name << " at " << conf << "\n";
    return 0;
}
