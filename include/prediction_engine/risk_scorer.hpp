#pragma once
#include <memory>
#include <vector>
#include <string>
#include "model_interface.hpp"

namespace stablecoin_tracker {
struct RiskAssessment {
    double risk_score;
    double price_deviation;
    double volume_anomaly;
    double liquidity_risk;
    double sentiment_risk;
    double tvl_risk;
    double sentiment_score;
};

class RiskScorer {
public:
    // Weights: volatility (30%), on-chain flow (30%), sentiment (20%), TVL change (20%)
    RiskScorer(std::shared_ptr<ModelInterface> model);
    void TrainModel(const std::vector<double>& features);
    RiskAssessment CalculateRisk(double volatility, double onchain, double sentiment, double tvl, double sentiment_score) const;
private:
    std::shared_ptr<ModelInterface> model_;
};
} 