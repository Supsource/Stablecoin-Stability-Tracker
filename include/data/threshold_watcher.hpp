#pragma once
#include "../core/types.hpp"
#include <vector>
#include <functional>

namespace stablecoin_tracker {

/**
 * @brief Threshold watcher for detecting stablecoin depeg events
 */
class ThresholdWatcher {
public:
    using AlertCallback = std::function<void(const Alert&)>;

    /**
     * @brief Constructor with configuration
     * @param config Configuration settings
     */
    explicit ThresholdWatcher(const Config& config);

    /**
     * @brief Add a new risk assessment to be monitored
     * @param assessment Risk assessment data
     */
    void AddRiskAssessment(const RiskAssessment& assessment);

    /**
     * @brief Check for threshold breaches and create alerts
     * @return List of generated alerts
     */
    std::vector<Alert> CheckThresholds();

    /**
     * @brief Set callback for real-time alert notifications
     * @param callback Alert callback function
     */
    void SetAlertCallback(AlertCallback callback);

    /**
     * @brief Update threshold configuration
     * @param config New configuration
     */
    void UpdateConfig(const Config& config);

    /**
     * @brief Clear all stored assessments
     */
    void Clear();

private:
    Config config_;
    std::vector<RiskAssessment> assessments_;
    AlertCallback alert_callback_;

    Alert GenerateAlert(const RiskAssessment& assessment);
    std::string DetermineAlertType(const RiskAssessment& assessment);
    double CalculateSeverity(const RiskAssessment& assessment);
    std::string GenerateAlertMessage(const RiskAssessment& assessment);
};

} // namespace stablecoin_tracker
