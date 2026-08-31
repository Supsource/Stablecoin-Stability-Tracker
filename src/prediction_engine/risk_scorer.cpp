#include "../../include/prediction_engine/risk_scorer.hpp"
#include "../../include/prediction_engine/model_interface.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace stablecoin_tracker {
RiskScorer::RiskScorer(std::shared_ptr<ModelInterface> model, RiskWeights weights)
    : model_(std::move(model)), weights_(weights) {}

void RiskScorer::TrainModel(const std::vector<double>& features) {
    if (model_) {
        model_->Train(features);
    }
}

RiskAssessment RiskScorer::CalculateRisk(double peg_deviation, double volatility, double onchain,
                                         double sentiment, double tvl) const {
    // 5% off peg maps to 1.0; return-vol of 2% maps to 1.0
    double p_norm = std::clamp(peg_deviation / 0.05, 0.0, 1.0);
    double v_norm = std::clamp(volatility / 0.02, 0.0, 1.0);
    // log10 volume: $10B-scale token units saturate
    double o_norm = std::clamp(std::log10(1.0 + std::max(onchain, 0.0)) / 10.0, 0.0, 1.0);
    // Only negative sentiment increases risk
    double s_norm = std::clamp(-sentiment, 0.0, 1.0);
    // Higher TVL lowers risk; $100M+ -> ~0 tvl risk
    double t_capacity = std::clamp(tvl / 1e8, 0.0, 1.0);
    double t_norm = 1.0 - t_capacity;

    double risk_score = weights_.peg_deviation * p_norm + weights_.volatility * v_norm +
                        weights_.onchain * o_norm + weights_.sentiment * s_norm +
                        weights_.tvl * t_norm;
    double model_pred = 0.0;
    if (model_) {
        model_pred = model_->Predict({p_norm, v_norm, o_norm, s_norm, t_norm});
    }
    if (std::isnan(risk_score) || std::isinf(risk_score)) {
        risk_score = 0.0;
    }
    risk_score = std::clamp(risk_score, 0.0, 1.0);

    std::ostringstream reason;
    reason << "peg_dev=" << p_norm << " vol=" << v_norm << " onchain=" << o_norm
           << " sentiment=" << s_norm << " tvl_risk=" << t_norm << " model=" << model_pred;

    RiskAssessment out;
    out.risk_score = risk_score;
    out.price_deviation = p_norm;
    out.volume_anomaly = o_norm;
    out.liquidity_risk = t_norm;
    out.sentiment_risk = s_norm;
    out.tvl_risk = t_norm;
    out.sentiment_score = sentiment;
    out.reasoning = reason.str();
    return out;
}
}
