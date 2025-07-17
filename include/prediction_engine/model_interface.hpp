#pragma once
#include <vector>

namespace stablecoin_tracker {
class ModelInterface {
public:
    virtual ~ModelInterface() = default;
    virtual void Train(const std::vector<double>& features) = 0;
    virtual double Predict(const std::vector<double>& features) = 0;
};

// Example statistical model
class StatisticalModel : public ModelInterface {
public:
    void Train(const std::vector<double>& features) override;
    double Predict(const std::vector<double>& features) override;
};
} 