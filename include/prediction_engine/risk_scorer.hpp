#pragma once
#include <memory>
#include <vector>
#include <string>
#include "model_interface.hpp"
#include "../core/types.hpp"

namespace stablecoin_tracker {
class RiskScorer {
public:
    explicit RiskScorer(std::shared_ptr<ModelInterface> model, RiskWeights weights = {});
    void TrainModel(const std::vector<double>& features);
    // peg_deviation: |price - peg|; volatility: return stddev; onchain: token-unit volume;
    // sentiment: [-1, 1] (negative increases risk); tvl: USD TVL (higher lowers risk)
    RiskAssessment CalculateRisk(double peg_deviation, double volatility, double onchain,
                                 double sentiment, double tvl) const;
private:
    std::shared_ptr<ModelInterface> model_;
    RiskWeights weights_;
};
}
