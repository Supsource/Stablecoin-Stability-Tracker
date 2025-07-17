#pragma once
#include <vector>
#include <string>
#include <map>
#include <chrono>

namespace stablecoin_tracker {
class HistoricalPatternEngine {
public:
    HistoricalPatternEngine(int lookback_hours);
    void AddPriceData(const std::string& symbol, const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& price_points);
    void AddPriceData(const std::string& symbol, const std::vector<double>& prices);
    std::vector<int> DetectDepegEvents(const std::string& symbol, double target_price, double threshold);
    double CalculateVolatility(const std::string& symbol);
    void SetHistoricalWindowDays(int days); // Set window in days
private:
    int historical_window_days_ = 30;
    std::map<std::string, std::vector<std::pair<std::chrono::system_clock::time_point, double>>> price_history_;
};
} 