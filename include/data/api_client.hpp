#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <queue>

namespace stablecoin_tracker {
class ApiClient {
public:
    ApiClient(const std::string& endpoint);
    std::vector<double> FetchCurrentPrices(const std::vector<std::string>& stablecoins);
    // Fetch historical data (batch)
    void FetchHistoricalData(const std::string& stablecoin, int hours);
    // Thread-safe queue for incoming data
    std::queue<std::string> data_queue;
private:
    std::string endpoint_;
    std::mutex queue_mutex_;
};
}
