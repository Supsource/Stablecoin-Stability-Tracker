#include "../../include/data/data_normalizer.hpp"
#include <algorithm>

namespace stablecoin_tracker {
std::vector<double> DataNormalizer::NormalizePrices(const std::vector<double>& raw_prices) {
    //  Simple min-max normalization
    if (raw_prices.empty()) return {};
    double min = *std::min_element(raw_prices.begin(), raw_prices.end());
    double max = *std::max_element(raw_prices.begin(), raw_prices.end());
    std::vector<double> norm;
    for (auto v : raw_prices) {
        norm.push_back((v - min) / (max - min + 1e-9));
    }
    return norm;
}

double DataNormalizer::NormalizeMetric(double raw_value) {
    // Identity normalization
    return raw_value;
}
}
