#include "../../include/alerting/notification_dispatcher.hpp"
#include <iostream>

namespace stablecoin_tracker {
NotificationDispatcher::NotificationDispatcher() {}

void NotificationDispatcher::DispatchAlerts(const std::vector<RiskAssessment>& alerts) {
    //   dispatching alerts as JSON
    for (const auto& a : alerts) {
        std::cout << "Dispatching alert: { \"risk_score\": " << a.risk_score << " }\n";
    }
}
}
