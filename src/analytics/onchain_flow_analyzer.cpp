#include "../../include/analytics/onchain_flow_analyzer.hpp"
#include <iostream>

namespace stablecoin_tracker {
OnChainFlowAnalyzer::OnChainFlowAnalyzer() : last_metric_(0.0) {}

void OnChainFlowAnalyzer::AnalyzeFlow(const std::string& token_address) {
    // simulate on-chain flow analysis
    std::cout << "Analyzing on-chain flow for " << token_address << "\n";
    last_metric_ = 42.0;
}

double OnChainFlowAnalyzer::GetLatestFlowMetric() const {
    return last_metric_;
}
}

