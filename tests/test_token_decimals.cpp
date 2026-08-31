#include "../include/data/etherscan_client.hpp"
#include <iostream>
#include <cmath>

using namespace stablecoin_tracker;

int main() {
    int passed = 0, total = 0;
    ++total;
    if (tokenDecimalsForSymbol("USDT") == 6 && tokenDecimalsForSymbol("USDC") == 6 &&
        tokenDecimalsForSymbol("DAI") == 18) {
        std::cout << "[PASS] decimals by symbol\n";
        ++passed;
    } else {
        std::cout << "[FAIL] decimals by symbol\n";
    }
    ++total;
    // 1 token with 6 decimals is 1_000_000 raw
    double usdt = rawAmountToTokens("1000000", 6);
    double dai = rawAmountToTokens("1000000000000000000", 18);
    if (std::abs(usdt - 1.0) < 1e-9 && std::abs(dai - 1.0) < 1e-6) {
        std::cout << "[PASS] raw amount conversion\n";
        ++passed;
    } else {
        std::cout << "[FAIL] conversion usdt=" << usdt << " dai=" << dai << "\n";
    }
    std::cout << "[TEST] token decimals passed " << passed << "/" << total << "\n";
    return passed == total ? 0 : 1;
}
