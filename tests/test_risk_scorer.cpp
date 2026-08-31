#include "../include/prediction_engine/risk_scorer.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>

using namespace stablecoin_tracker;

static bool near(double a, double b) { return std::abs(a - b) < 1e-6; }

int main() {
    auto model = std::make_shared<StatisticalModel>();
    RiskScorer scorer(model);
    int passed = 0, total = 0;

    ++total;
    auto r1 = scorer.CalculateRisk(0, 0, 0, 0, 0);
    // zero TVL -> tvl_risk 1.0 * 0.15
    if (near(r1.risk_score, 0.15) && near(r1.tvl_risk, 1.0) && near(r1.sentiment_risk, 0.0)) {
        std::cout << "[PASS] Test 1: Zeros (no liquidity is risky, no negative sentiment)\n";
        ++passed;
    } else {
        std::cout << "[FAIL] Test 1: got risk=" << r1.risk_score << " tvl=" << r1.tvl_risk
                  << " sent=" << r1.sentiment_risk << "\n";
    }

    ++total;
    auto r_neg = scorer.CalculateRisk(0, 0, 0, -0.8, 1e8);
    auto r_pos = scorer.CalculateRisk(0, 0, 0, 0.8, 1e8);
    if (r_neg.risk_score > r_pos.risk_score && r_neg.sentiment_risk > r_pos.sentiment_risk) {
        std::cout << "[PASS] Test 2: Negative sentiment increases risk vs positive\n";
        ++passed;
    } else {
        std::cout << "[FAIL] Test 2: neg=" << r_neg.risk_score << " pos=" << r_pos.risk_score << "\n";
    }

    ++total;
    auto r_low_tvl = scorer.CalculateRisk(0, 0, 0, 0, 0);
    auto r_high_tvl = scorer.CalculateRisk(0, 0, 0, 0, 1e8);
    if (r_high_tvl.risk_score < r_low_tvl.risk_score && near(r_high_tvl.tvl_risk, 0.0)) {
        std::cout << "[PASS] Test 3: Higher TVL lowers risk\n";
        ++passed;
    } else {
        std::cout << "[FAIL] Test 3: low_tvl=" << r_low_tvl.risk_score
                  << " high_tvl=" << r_high_tvl.risk_score << "\n";
    }

    ++total;
    auto r_peg = scorer.CalculateRisk(0.05, 0, 0, 0, 1e8);
    auto r_flat = scorer.CalculateRisk(0.0, 0, 0, 0, 1e8);
    if (r_peg.price_deviation > 0.99 && r_peg.risk_score > r_flat.risk_score) {
        std::cout << "[PASS] Test 4: Peg deviation is scored\n";
        ++passed;
    } else {
        std::cout << "[FAIL] Test 4: peg_dev field=" << r_peg.price_deviation
                  << " scores " << r_peg.risk_score << " vs " << r_flat.risk_score << "\n";
    }

    ++total;
    auto r_vol = scorer.CalculateRisk(0, 0.02, 0, 0, 1e8);
    if (r_vol.risk_score > r_flat.risk_score) {
        std::cout << "[PASS] Test 5: Volatility increases risk after scaling\n";
        ++passed;
    } else {
        std::cout << "[FAIL] Test 5: vol score " << r_vol.risk_score << "\n";
    }

    std::cout << "[TEST] RiskScorer passed " << passed << "/" << total << " test cases\n";
    return passed == total ? 0 : 1;
}
