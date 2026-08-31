#pragma once
#include <vector>
#include <string>

namespace stablecoin_tracker {
class DataNormalizer {
public:
    // Normalize raw price data
    std::vector<double> NormalizePrices(const std::vector<double>& raw_prices);
    double NormalizeMetric(double raw_value);
    // Normalize prices as distance from peg, scaled so max_dev maps to 1.0
    std::vector<double> NormalizePegDeviations(const std::vector<double>& prices,
                                              double peg = 1.0, double max_dev = 0.05);

};
}
