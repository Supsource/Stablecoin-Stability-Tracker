#include "../../include/data/sentiment_client.hpp"
#include <iostream>

namespace stablecoin_tracker {
SentimentFetch fetchSentimentScore(const std::string& symbol) {
    std::cout << "[INFO] [MOCK] Sentiment for " << symbol
              << " is a placeholder (-0.35). Wire a real API to replace this.\n";
    return {-0.35, true};
}
}
