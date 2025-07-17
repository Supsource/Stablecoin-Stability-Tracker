#include "../../include/data/data_normalizer.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

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
    return 0;
} 