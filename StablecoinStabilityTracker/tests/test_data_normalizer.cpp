#include "../../include/data/data_normalizer.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

using namespace stablecoin_tracker;

void test_normalize_prices() {
    DataNormalizer normalizer;
    std::vector<double> raw = {1.0, 2.0, 3.0};
    auto norm = normalizer.NormalizePrices(raw);
    assert(norm.size() == 3);
    assert(std::fabs(norm[0] - 0.0) < 1e-6);
    assert(std::fabs(norm[2] - 1.0) < 1e-6);
    std::cout << "test_normalize_prices passed\n";
}

int main() {
    test_normalize_prices();
    DataNormalizer normalizer;
    auto peg = normalizer.NormalizePegDeviations({1.0, 0.95, 1.05}, 1.0, 0.05);
    assert(std::fabs(peg[0] - 0.0) < 1e-6);
    assert(std::fabs(peg[1] - 1.0) < 1e-6);
    std::cout << "test_normalize_peg_deviations passed\n";
    return 0;
} 