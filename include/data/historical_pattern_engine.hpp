#pragma once
#include "../core/types.hpp"
#include "../utils/time_series.hpp"
#include <vector>
#include <map>
#include <memory>

namespace stablecoin_tracker {
class HistoricalPatternEngine {
public:
    explicit HistoricalPatternEngine(int lookback_hours = 168);
    void AddPriceData(const std::string& symbol, const std::vector<PriceData>& price_data);

    /**
     * @brief Detect depeg patterns in historical data
     * @param symbol Stablecoin symbol
     * @param target_price Target price (usually 1.0)
     * @param threshold Depeg threshold
     * @return Vector of detected depeg events
     */
    std::vector<std::chrono::system_clock::time_point> DetectDepegEvents(
        const std::string& symbol, double target_price, double threshold);

    /**
     * @brief Calculate price volatility
     * @param symbol Stablecoin symbol
     * @param window_hours Time window for volatility calculation
     * @return Volatility measure
     */
    double CalculateVolatility(const std::string& symbol, int window_hours = 24);

    /**
     * @brief Calculate rolling Z-score for price deviation
     * @param symbol Stablecoin symbol
     * @param window_size Rolling window size
     * @return Vector of Z-scores
     */
    std::vector<double> CalculateRollingZScore(const std::string& symbol, int window_size = 50);

    /**
     * @brief Detect volume spikes
     * @param symbol Stablecoin symbol
     * @param spike_threshold Threshold for volume spike detection
     * @return Vector of volume spike timestamps
     */
    std::vector<std::chrono::system_clock::time_point> DetectVolumeSpikes(
        const std::string& symbol, double spike_threshold = 2.0);

    /**
     * @brief Get correlation with other stablecoins
     * @param symbol1 First stablecoin symbol
     * @param symbol2 Second stablecoin symbol
     * @return Correlation coefficient
     */
    double GetCorrelation(const std::string& symbol1, const std::string& symbol2);

    /**
     * @brief Clear historical data
     */
    void ClearData();

private:
    int lookback_hours_;
    std::map<std::string, TimeSeries<PriceData>> price_series_;
    
    std::vector<double> ExtractPrices(const std::vector<PriceData>& data);
    std::vector<double> ExtractVolumes(const std::vector<PriceData>& data);
    double CalculateStandardDeviation(const std::vector<double>& values);
    double CalculateMean(const std::vector<double>& values);
    double CalculateCorrelationCoefficient(const std::vector<double>& x, const std::vector<double>& y);
};

} 
