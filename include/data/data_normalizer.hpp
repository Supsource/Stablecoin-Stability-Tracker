#pragma once
#include <vector>
#include <string>

namespace stablecoin_tracker {
class DataNormalizer {
public:
    // Normalize raw price data
    std::vector<double> NormalizePrices(const std::vector<double>& raw_prices);
    // Normalize on-chain or sentiment data 
    double NormalizeMetric(double raw_value);
};
}
