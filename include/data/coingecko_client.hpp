#pragma once
#include <string>
#include <vector>
#include <utility>

namespace stablecoin_tracker {
std::vector<std::pair<long long, double>> fetchHistoricalPrices(
    const std::string& coinGeckoId,
    int days = 30
);
} 