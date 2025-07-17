#include "../../include/data/defi_client.hpp"
#include <iostream>

namespace stablecoin_tracker {
double fetchTVL(const std::string& coinGeckoId) {
    // TODO: Integrate real Aave/Uniswap API.
    std::cout << "[INFO] Fetching TVL for " << coinGeckoId << "\n";
    return 50000000.0; // Mock: $50M TVL
}
} 