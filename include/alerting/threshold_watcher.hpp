#pragma once
#include <vector>
#include <string>
#include "../prediction_engine/risk_scorer.hpp"

namespace stablecoin_tracker {
class ThresholdWatcher {
public:
    ThresholdWatcher(double risk_threshold);
    void AddRiskAssessment(const RiskAssessment& assessment);
    std::vector<RiskAssessment> CheckThresholds();
private:
    double risk_threshold_;
    std::vector<RiskAssessment> assessments_;
};
} 