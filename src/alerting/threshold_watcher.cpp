#include "../../include/alerting/threshold_watcher.hpp"
#include <iostream>

namespace stablecoin_tracker {
ThresholdWatcher::ThresholdWatcher(double risk_threshold) : risk_threshold_(risk_threshold) {}

void ThresholdWatcher::AddRiskAssessment(const RiskAssessment& assessment) {
    // Store assessment
    assessments_.push_back(assessment);
}

std::vector<RiskAssessment> ThresholdWatcher::CheckThresholds() {
    // Return assessments above threshold
    std::vector<RiskAssessment> alerts;
    for (const auto& a : assessments_) {
        if (a.risk_score > risk_threshold_) {
            alerts.push_back(a);
        }
    }
    std::cout << "Checked thresholds, found " << alerts.size() << " alerts\n";
    return alerts;
}
}

