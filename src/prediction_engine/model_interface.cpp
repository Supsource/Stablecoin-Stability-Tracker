#include "../../include/prediction_engine/model_interface.hpp"
#include <algorithm>
#include <numeric>

namespace stablecoin_tracker {
void StatisticalModel::Train(const std::vector<double>& /*features*/) {}

double StatisticalModel::Predict(const std::vector<double>& features) {
    if (features.empty()) return 0.0;
    double sum = std::accumulate(features.begin(), features.end(), 0.0);
    return std::clamp(sum / static_cast<double>(features.size()), 0.0, 1.0);
}
}
