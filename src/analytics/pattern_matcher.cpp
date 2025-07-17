#include "../../include/analytics/pattern_matcher.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace stablecoin_tracker {
PatternMatcher::PatternMatcher(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& price_data)
    : price_data_(price_data) {}

// Compute rolling window features here
std::vector<std::vector<double>> PatternMatcher::computeFeatureWindows(size_t window_size) const {
    std::vector<std::vector<double>> features;
    if (price_data_.size() < window_size) return features;
    for (size_t i = 0; i + window_size <= price_data_.size(); ++i) {
        std::vector<double> window;
        for (size_t j = 0; j < window_size; ++j) {
            window.push_back(price_data_[i + j].second);
        }
        double mean = std::accumulate(window.begin(), window.end(), 0.0) / window.size();
        double sq_sum = std::inner_product(window.begin(), window.end(), window.begin(), 0.0);
        double stddev = std::sqrt(sq_sum / window.size() - mean * mean);
        double z = (window.back() - mean) / (stddev + 1e-9);
        double mean_shift = (window.back() - window.front()) / (window.front() + 1e-9);
        features.push_back({mean, stddev, z, mean_shift});
    }
    return features;
}
double PatternMatcher::cosineSimilarity(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0;
    double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    if (norm_a == 0.0 || norm_b == 0.0) return 0.0;
    return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

double PatternMatcher::euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size() || a.empty()) return 1e9;
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return std::sqrt(sum);
}

// Compare the most recent window to past depeg patterns
// Returns confidence score (0.0-1.0), true if above threshold
// matched_pattern (optional) will be set to the best match
// Uses cosine similarity (can switch to Euclidean if desired)
double PatternMatcher::matchDepegPattern(const std::vector<std::vector<double>>& past_patterns, double threshold, std::string* matched_pattern) const {
    if (price_data_.empty() || past_patterns.empty()) return 0.0;
    size_t window_size = past_patterns[0].size();
    if (price_data_.size() < window_size) return 0.0;
    std::vector<double> recent_window;
    for (size_t i = price_data_.size() - window_size; i < price_data_.size(); ++i) {
        recent_window.push_back(price_data_[i].second);
    }
    double best_score = 0.0;
    for (const auto& pattern : past_patterns) {
        double score = cosineSimilarity(recent_window, pattern);
        if (score > best_score) {
            best_score = score;
            if (matched_pattern) *matched_pattern = "Pattern"; 
        }
    }
    return best_score;
}

// known depeg event patterns 
std::vector<std::vector<double>> PatternMatcher::getKnownDepegPatterns() {
    return {
        {1.0, 0.99, 0.97, 0.95, 0.98, 1.0}, // USDT depeg
        {1.0, 0.98, 0.92, 0.85, 0.7, 0.2}    // UST collapse
    };
}

} 

#ifdef PATTERN_MATCHER_MAIN
#include <ctime>
int main() {
    using namespace stablecoin_tracker;
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> prices;
    auto now = std::chrono::system_clock::now();
    for (int i = 0; i < 10; ++i) prices.emplace_back(now + std::chrono::seconds(i), 1.0);
    for (int i = 0; i < 6; ++i) prices.emplace_back(now + std::chrono::seconds(10 + i), 1.0 - 0.01 * i);
    PatternMatcher matcher(prices);
    auto features = matcher.computeFeatureWindows(6);
    auto patterns = PatternMatcher::getKnownDepegPatterns();
    double conf = matcher.matchDepegPattern(patterns, 0.7);
    std::cout << "Pattern match confidence: " << conf << std::endl;
    if (conf > 0.7) std::cout << "Depeg pattern detected!\n";
    else std::cout << "No depeg pattern detected.\n";
    return 0;
}
#endif 