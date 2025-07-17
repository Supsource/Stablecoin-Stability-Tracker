#pragma once
#include "../core/types.hpp"
#include "../utils/logger.hpp"
#include <vector>
#include <memory>

namespace stablecoin_tracker {

/**
 * @brief Interface for statistics and machine learning models
 */
class ModelInterface {
public:
    virtual ~ModelInterface() = default;

    /**
     * @brief Train the model with market data
     * @param data Market data snapshot
     */
    virtual void Train(const MarketData& data) = 0;

    /**
     * @brief Predict risk score based on market data
     * @param data Market data snapshot
     * @return Predicted risk score
     */
    virtual double PredictRiskScore(const MarketData& data) = 0;

    /**
     * @brief Update model with new data
     * @param data Market data snapshot
     */
    virtual void Update(const MarketData& data) = 0;

    /**
     * @brief Reset the model state
     */
    virtual void Reset() = 0;
};

/**
 * @brief Risk scorer for calculating depeg risk
 */
class RiskScorer {
public:
    /**
     * @brief Constructor with model interface
     * @param model Shared pointer to model interface
     */
    explicit RiskScorer(std::shared_ptr<ModelInterface> model);

    /**
     * @brief Calculate risk assessment
     * @param data Market data snapshot
     * @return Risk assessment result
     */
    RiskAssessment CalculateRisk(const MarketData& data);

    /**
     * @brief Train risk model
     * @param data Market data
     */
    void TrainModel(const MarketData& data);

    /**
     * @brief Update risk model with new data
     * @param data Market data
     */
    void UpdateModel(const MarketData& data);

    /**
     * @brief Reset risk model
     */
    void ResetModel();

private:
    std::shared_ptr<ModelInterface> model_;
};

} // namespace stablecoin_tracker
