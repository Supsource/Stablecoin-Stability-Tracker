#pragma once
#include <vector>
#include <string>
#include <utility>
#include <chrono>

namespace stablecoin_tracker {
struct DepegPattern {
    std::string name;
    std::vector<double> prices;
};

class PatternMatcher {
public:
    PatternMatcher(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& price_data);
    std::vector<std::vector<double>> computeFeatureWindows(size_t window_size) const;
    // Returns cosine similarity of the latest window vs known patterns (0-1).
    // If best score is below threshold, matched_pattern is left unchanged and 0.0 is returned.
    double matchDepegPattern(const std::vector<DepegPattern>& past_patterns, double threshold,
                             std::string* matched_pattern = nullptr) const;
    static double cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b);
    static double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<DepegPattern> getKnownDepegPatterns();
private:
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> price_data_;
};
}
