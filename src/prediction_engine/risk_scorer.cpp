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

RiskAssessment RiskScorer::CalculateRisk(double volatility, double onchain, double sentiment, double tvl, double sentiment_score) {
    double v_norm = std::min(volatility, 1.0);
    double o_norm = std::min(onchain, 1.0);
    double s_norm = std::min(std::abs(sentiment), 1.0); // sentiment can be negative
    double t_norm = std::min(tvl, 1.0);
    // log normalized and capped values
    std::cout << "[DEBUG] Scoring - Volatility: " << volatility << " (capped: " << v_norm << ")"
              << ", Onchain: " << onchain << " (capped: " << o_norm << ")"
              << ", Sentiment: " << sentiment << " (capped: " << s_norm << ")"
              << ", TVL: " << tvl << " (capped: " << t_norm << ")" << std::endl;

    double risk_score = 0.3 * v_norm + 0.3 * o_norm + 0.2 * (1.0 - s_norm) + 0.2 * t_norm;
    // this handles NaN/Inf
    if (std::isnan(risk_score) || std::isinf(risk_score)) {
        std::cerr << "[WARN] Risk score is NaN or Inf. Defaulting to 0.0\n";
        risk_score = 0.0;
    }
    std::cout << "[RISK] Volatility: " << volatility << " (30%), On-chain: " << onchain << " (30%), Sentiment: " << sentiment << " (20%), TVL: " << tvl << " (20%)\n";
    std::cout << "[RISK] Sentiment score: " << sentiment_score << (sentiment_score < 0 ? " (bearish)" : " (bullish)") << ", Risk adjusted by " << (1.0 - sentiment) * 0.2 << "\n";
    return {risk_score, v_norm, o_norm, t_norm, (1.0 - s_norm) * 0.2, t_norm, sentiment_score};
}
}
