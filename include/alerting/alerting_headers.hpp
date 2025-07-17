#pragma once
#include "../core/types.hpp"
#include "../utils/logger.hpp"
#include <string>
#include <chrono>
#include <vector>

namespace stablecoin_tracker {
class ThresholdWatcher {
public:
    void AddRiskAssessment(const RiskAssessment& assessment);

    std::vector<Alert> CheckThresholds();

private:
    std::vector<RiskAssessment> assessments_;

    Alert GenerateAlert(const RiskAssessment& assessment);
};
class NotificationDispatcher {
public:
    void DispatchAlerts(const std::vector<Alert>& alerts);
    void ConfigureEmail(bool enabled, const std::string& smtp_server, int smtp_port);
    void ConfigureWebhook(bool enabled, const std::string& url);

private:
    bool email_enabled_;
    std::string smtp_server_;
    int smtp_port_;

    bool webhook_enabled_;
    std::string webhook_url_;

    void SendEmail(const Alert& alert);
    void SendWebhook(const Alert& alert);
};

} 

