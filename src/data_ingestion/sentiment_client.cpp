#include "../../include/data/sentiment_client.hpp"
#include <iostream>

namespace stablecoin_tracker {
double fetchSentimentScore(const std::string& symbol) {
    // TODO: Integrate real Twitter/Reddit API.
    std::cout << "[INFO] Fetching sentiment score for " << symbol << "\n";
    return -0.35; 
}
} 