#pragma once
#include "../core/types.hpp"
#include "../utils/logger.hpp"
#include "../utils/json_parser.hpp"
#include <string>
#include <vector>

namespace stablecoin_tracker {

/**
 * @brief Notification dispatcher for sending alerts
 */
class NotificationDispatcher {
public:
    /**
     * @brief Constructor with configuration
     * @param config Configuration settings
     */
    explicit NotificationDispatcher(const Config& config);

    /**
     * @brief Send alerts via email and/or webhook
     * @param alerts List of alerts to send
     */
    void DispatchAlerts(const std::vector<Alert>& alerts);

    /**
     * @brief Send single alert
     * @param alert Alert to send
     */
    void DispatchAlert(const Alert& alert);

    /**
     * @brief Configure email settings
     * @param enabled Enable email notifications
     * @param smtp_server SMTP server address
     * @param smtp_port SMTP server port
     */
    void ConfigureEmail(bool enabled, const std::string& smtp_server, int smtp_port);

    /**
     * @brief Configure webhook settings
     * @param enabled Enable webhook notifications
     * @param url Webhook target URL
     */
    void ConfigureWebhook(bool enabled, const std::string& url);

    /**
     * @brief Update configuration
     * @param config New configuration
     */
    void UpdateConfig(const Config& config);

private:
    Config config_;
    bool email_enabled_;
    std::string smtp_server_;
    int smtp_port_;
    bool webhook_enabled_;
    std::string webhook_url_;

    void SendEmail(const Alert& alert);
    void SendWebhook(const Alert& alert);
    std::string AlertToJson(const Alert& alert);
    std::string FormatEmailSubject(const Alert& alert);
    std::string FormatEmailBody(const Alert& alert);
};

} // namespace stablecoin_tracker
