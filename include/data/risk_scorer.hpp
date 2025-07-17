#pragma once
#include "../core/types.hpp"
#include "model_interface.hpp"
#include <memory>

namespace stablecoin_tracker {

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
     * @brief Calculate price deviation risk component
     * @param prices Price data vector
     * @param target_price Target price (usually 1.0)
     * @return Price deviation risk score
     */
    double CalculatePriceDeviationRisk(const std::vector<PriceData>& prices, double target_price = 1.0);

    /**
     * @brief Calculate volume anomaly risk component
     * @param prices Price data vector
     * @return Volume anomaly risk score
     */
    double CalculateVolumeAnomalyRisk(const std::vector<PriceData>& prices);

    /**
     * @brief Calculate liquidity risk component
     * @param on_chain_data On-chain transaction data
     * @return Liquidity risk score
     */
    double CalculateLiquidityRisk(const std::vector<OnChainData>& on_chain_data);

    /**
     * @brief Calculate sentiment risk component
     * @param sentiment_data Sentiment data vector
     * @return Sentiment risk score
     */
    double CalculateSentimentRisk(const std::vector<SentimentData>& sentiment_data);

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
    double Normalize(double value, double min_val, double max_val);
    double CalculateMovingAverage(const std::vector<double>& values, int window_size);
    double CalculateStandardDeviation(const std::vector<double>& values);
};

} // namespace stablecoin_tracker
