#include "../../include/analytics/historical_pattern_engine.hpp"
#include <cassert>
#include <iostream>

using namespace stablecoin_tracker;

void test_depeg_detection() {
    HistoricalPatternEngine engine(24);
    std::vector<double> prices = {1.0, 1.0, 0.95, 1.0};
    engine.AddPriceData("USDT", prices);
    auto depegs = engine.DetectDepegEvents("USDT", 1.0, 0.02);
    assert(!depegs.empty());
    std::cout << "test_depeg_detection passed\n";
}

int main() {
    test_depeg_detection();
    return 0;
} 