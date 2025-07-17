#include "../../include/prediction_engine/risk_scorer.hpp"
#include "../../include/prediction_engine/model_interface.hpp"
#include <iostream>
#include <cmath>

namespace stablecoin_tracker {
RiskScorer::RiskScorer(std::shared_ptr<ModelInterface> model) : model_(model) {}

void RiskScorer::TrainModel(const std::vector<double>& features) {
    // Train the model
    std::cout << "Training risk scorer...\n";
    model_->Train(features);
}

RiskAssessment RiskScorer::CalculateRisk(double volatility, double onchain, double sentiment, double tvl, double sentiment_score) const {
    // Improved normalization with epsilon to avoid division by zero
    constexpr double EPS = 1e-9;
    double v_norm = std::clamp(volatility, 0.0, 1.0);
    double o_norm = std::clamp(onchain, 0.0, 1.0);
    double s_norm = std::clamp(std::abs(sentiment), 0.0, 1.0);
    double t_norm = std::clamp(tvl, 0.0, 1.0);

    // Calculate risk score
    double risk_score = 0.3 * v_norm + 0.3 * o_norm + 0.2 * (1.0 - s_norm) + 0.2 * t_norm;
    if (std::isnan(risk_score) || std::isinf(risk_score)) {
        risk_score = 0.0;
    }
    return {risk_score, v_norm, o_norm, t_norm, (1.0 - s_norm) * 0.2, t_norm, sentiment_score};
}
}
