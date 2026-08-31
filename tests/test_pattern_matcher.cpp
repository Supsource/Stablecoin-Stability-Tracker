#include "../include/analytics/pattern_matcher.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>

using namespace stablecoin_tracker;

int main() {
    int passed = 0, total = 0;

    ++total;
    auto sim = PatternMatcher::cosineSimilarity({1, 0, 0}, {1, 0, 0});
    if (std::abs(sim - 1.0) < 1e-9) {
        std::cout << "[PASS] cosine identical vectors\n";
        ++passed;
    } else {
        std::cout << "[FAIL] cosine identical\n";
    }

    ++total;
    auto now = std::chrono::system_clock::now();
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> pegged;
    for (int i = 0; i < 8; ++i) {
        pegged.emplace_back(now + std::chrono::seconds(i), 1.0);
    }
    PatternMatcher stable(pegged);
    DepegPattern collapse{"TerraUSD collapse (May 2022)", {1.0, 0.98, 0.92, 0.85, 0.7, 0.2}};
    double conf = stable.matchDepegPattern({collapse}, 0.99);
    if (conf == 0.0) {
        std::cout << "[PASS] flat peg does not match collapse pattern at high threshold\n";
        ++passed;
    } else {
        std::cout << "[FAIL] flat peg matched with " << conf << "\n";
    }

    ++total;
    auto features = stable.computeFeatureWindows(4);
    bool nan = false;
    for (const auto& f : features) {
        for (double x : f) {
            if (std::isnan(x)) nan = true;
        }
    }
    if (!nan && !features.empty()) {
        std::cout << "[PASS] feature windows have no NaN\n";
        ++passed;
    } else {
        std::cout << "[FAIL] feature windows nan=" << nan << " size=" << features.size() << "\n";
    }

    std::cout << "[TEST] PatternMatcher passed " << passed << "/" << total << "\n";
    return passed == total ? 0 : 1;
}
