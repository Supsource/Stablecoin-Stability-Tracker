#include "../../include/data/defi_client.hpp"
#include <iostream>

namespace stablecoin_tracker {
TvlFetch fetchTVL(const std::string& coinGeckoId) {
    std::cout << "[INFO] [MOCK] TVL for " << coinGeckoId
              << " is a placeholder ($50M). Wire Aave/Uniswap to replace this.\n";
    return {50000000.0, true};
}
}
