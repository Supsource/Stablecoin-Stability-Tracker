#pragma once
#include <vector>
#include <string>
#include "../prediction_engine/risk_scorer.hpp"

namespace stablecoin_tracker {
class NotificationDispatcher {
public:
    NotificationDispatcher();
    void DispatchAlerts(const std::vector<RiskAssessment>& alerts);
};
} 