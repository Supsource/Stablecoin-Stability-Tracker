#pragma once
#include <string>

namespace stablecoin_tracker {
class OnChainFlowAnalyzer {
public:
    OnChainFlowAnalyzer();
    void AnalyzeFlow(const std::string& token_address);
    double GetLatestFlowMetric() const;
private:
    double last_metric_;
};
} 