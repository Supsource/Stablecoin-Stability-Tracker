#pragma once
#include <string>

namespace stablecoin_tracker {
struct OnChainMetrics {
    double tx_volume = 0.0;          // token units (not wei)
    int large_transfer_count = 0;    // transfers >= 1M tokens
    int active_wallet_count = 0;
};

int tokenDecimalsForSymbol(const std::string& symbol);
double rawAmountToTokens(const std::string& raw_value, int decimals);

// Fetch on-chain metrics for a token symbol over the past N days.
OnChainMetrics fetchOnChainMetrics(const std::string& symbol, int days = 30, const std::string& etherscan_api_key = "");
}
