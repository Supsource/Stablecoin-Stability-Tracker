#pragma once
#include "../core/types.hpp"

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
 * @brief Simple statistical model implementation
 */
class StatisticalModel : public ModelInterface {
public:
    StatisticalModel();
    ~StatisticalModel() override = default;

    void Train(const MarketData& data) override;
    double PredictRiskScore(const MarketData& data) override;
    void Update(const MarketData& data) override;
    void Reset() override;

private:
    bool is_trained_;
    double baseline_volatility_;
    double baseline_volume_;
    double baseline_sentiment_;
    
    double CalculateVolatility(const std::vector<PriceData>& prices);
    double CalculateAverageVolume(const std::vector<PriceData>& prices);
    double CalculateAverageSentiment(const std::vector<SentimentData>& sentiment);
};

} // namespace stablecoin_tracker
