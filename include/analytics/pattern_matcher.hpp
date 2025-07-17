#pragma once
#include <vector>
#include <string>
#include <utility>
#include <chrono>

namespace stablecoin_tracker {
class PatternMatcher {
public:
    PatternMatcher(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& price_data);
    std::vector<std::vector<double>> computeFeatureWindows(size_t window_size) const;
    double matchDepegPattern(const std::vector<std::vector<double>>& past_patterns, double threshold, std::string* matched_pattern = nullptr) const;
    static double cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b);
    static double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b);
    static std::vector<std::vector<double>> getKnownDepegPatterns();
private:
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> price_data_;
};
} 