#include "../../include/data/data_normalizer.hpp"
#include <algorithm>
#include <cmath>

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
    return raw_value;
}

std::vector<double> DataNormalizer::NormalizePegDeviations(const std::vector<double>& prices,
                                                          double peg, double max_dev) {
    std::vector<double> out;
    out.reserve(prices.size());
    double scale = max_dev > 0.0 ? max_dev : 0.05;
    for (auto p : prices) {
        out.push_back(std::min(1.0, std::abs(p - peg) / scale));
    }
    return out;
}

}
