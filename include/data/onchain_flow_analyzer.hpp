#pragma once
#include "../core/types.hpp"
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace stablecoin_tracker {

/**
 * @brief Analyze on-chain flows for stability patterns
 */
class OnChainFlowAnalyzer {
public:
    /**
     * @brief Analyze token transfers for stability events
     * @param symbol Stablecoin symbol
     * @param on_chain_data On-chain transaction data
     */
    void AnalyzeTransfers(const std::string& symbol, const std::vector<OnChainData>& on_chain_data);

    /**
     * @brief Identify suspicious whale movements
     * @param symbol Stablecoin symbol
     * @return List of suspicious transactions
     */
    std::vector<OnChainData> IdentifyWhaleMovements(const std::string& symbol);

    /**
     * @brief Calculate liquidity inflow/outflow
     * @param symbol Stablecoin symbol
     * @return Net liquidity flow
     */
    double CalculateLiquidityFlow(const std::string& symbol);

    /**
     * @brief Get total transaction volume for the symbol
     * @param symbol Stablecoin symbol
     * @param past_hours Time range in the past to analyze
     * @return Total volume
     */
    double GetTotalVolume(const std::string& symbol, int past_hours = 24);

    /**
     * @brief Clear all analyzed data
     */
    void ClearData();

private:
    std::map<std::string, std::vector<OnChainData>> on_chain_transactions_;
    
    double CalculateVolume(const std::vector<OnChainData>& transactions);
};

} // namespace stablecoin_tracker
