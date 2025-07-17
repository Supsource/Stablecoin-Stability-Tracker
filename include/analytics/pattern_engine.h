#pragma once
#include <vector>

namespace stablecoin_tracker {
struct PricePoint {
    long long timestamp;
    double price;
};
void detectHistoricalAnomalies(const std::vector<PricePoint>& history);
} 