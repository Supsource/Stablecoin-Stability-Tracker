// Unit tests for RiskScorer in Stablecoin Stability Tracker
#include "../include/prediction_engine/risk_scorer.hpp"
#include <iostream>
#include <cassert>
#include <cmath> 

using namespace stablecoin_tracker;

int main() {
    auto model = std::make_shared<StatisticalModel>();
    RiskScorer scorer(model);
    int passed = 0, total = 0;
    // Test 1: All zeros
    ++total;
    auto r1 = scorer.CalculateRisk(0, 0, 0, 0, 0);
    double expected1 = 0.3 * 0 + 0.3 * 0 + 0.2 * 1.0 + 0.2 * 0;
    if (std::abs(r1.risk_score - expected1) < 1e-6) {
        std::cout << "[PASS] Test 1: All zeros\n"; ++passed;
    } else {
        std::cout << "[FAIL] Test 1: All zeros\n";
    }
    // Test 2: All ones
    ++total;
    auto r2 = scorer.CalculateRisk(1, 1, 1, 1, 1);
    double expected2 = 0.3 * 1 + 0.3 * 1 + 0.2 * 0 + 0.2 * 1;
    if (std::abs(r2.risk_score - expected2) < 1e-6) {
        std::cout << "[PASS] Test 2: All ones\n"; ++passed;
    } else {
        std::cout << "[FAIL] Test 2: All ones\n";
    }
    // Test 3: Mixed
    ++total;
    auto r3 = scorer.CalculateRisk(0.5, 0.8, -0.3, 0.6, -0.3);
    if (r3.risk_score >= 0 && r3.risk_score <= 1.0) {
        std::cout << "[PASS] Test 3: Mixed values in [0,1]\n"; ++passed;
    } else {
        std::cout << "[FAIL] Test 3: Mixed values in [0,1]\n";
    }
    std::cout << "[TEST] RiskScorer passed " << passed << "/" << total << " test cases\n";
    return passed == total ? 0 : 1;
} 