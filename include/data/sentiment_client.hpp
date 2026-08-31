#pragma once
#include <string>

namespace stablecoin_tracker {
struct SentimentFetch {
    double score = 0.0;
    bool is_mock = true;
};
SentimentFetch fetchSentimentScore(const std::string& symbol);
}
