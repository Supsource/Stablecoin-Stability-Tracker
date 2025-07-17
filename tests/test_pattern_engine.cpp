// Unit test for historical pattern matching stub
#include "../include/analytics/pattern_engine.h"
#include <iostream>
#include <vector>

using namespace stablecoin_tracker;

int main() {
    std::vector<PricePoint> history = {
        {1620000000, 1.00}, {1620003600, 0.99}, {1620007200, 0.98}, {1620010800, 0.97}
    };
    std::cout << "[TEST] Running detectHistoricalAnomalies 5 times to check random output...\n";
    for (int i = 0; i < 5; ++i) {
        detectHistoricalAnomalies(history);
    }
    std::cout << "[TEST] Pattern engine test complete.\n";
    return 0;
} 