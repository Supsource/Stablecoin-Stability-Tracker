#include "../../include/data/api_client.hpp"
#include <iostream>

namespace stablecoin_tracker {
ApiClient::ApiClient(const std::string& endpoint) : endpoint_(endpoint) {
    // Initialize REST/WebSocket clients here
}

std::vector<double> ApiClient::FetchCurrentPrices(const std::vector<std::string>& stablecoins) {
    //Simulate fetching prices
    std::vector<double> prices(stablecoins.size(), 1.0);
    return prices;
}

void ApiClient::FetchHistoricalData(const std::string& stablecoin, int hours) {
    std::cout << "Fetching historical data for " << stablecoin << " (" << hours << "h)\n";
}
}

