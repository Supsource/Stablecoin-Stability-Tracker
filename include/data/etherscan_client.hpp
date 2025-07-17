#pragma once
#include <string>

namespace stablecoin_tracker {
struct OnChainMetrics {
    double tx_volume = 0.0;
    int large_transfer_count = 0;
};
// Fetch on-chain metrics for a token address over the past N days.
OnChainMetrics fetchOnChainMetrics(const std::string& tokenAddress, int days = 30, const std::string& etherscan_api_key = "");
} 