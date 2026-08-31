#include "../../include/alerting/notification_dispatcher.hpp"
#include <iostream>

namespace stablecoin_tracker {
NotificationDispatcher::NotificationDispatcher() {}

void NotificationDispatcher::DispatchAlerts(const std::vector<RiskAssessment>& alerts) {
    for (const auto& a : alerts) {
        std::cout << "Dispatching alert: { \"symbol\": \"" << a.symbol
                  << "\", \"risk_score\": " << a.risk_score
                  << ", \"reason\": \"" << a.reasoning << "\" }\n";
    }
}
}
