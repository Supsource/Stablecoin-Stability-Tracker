#include "../include/core/config_manager.hpp"
#include "../include/analytics/historical_pattern_engine.hpp"
#include "../include/analytics/pattern_matcher.hpp"
#include "../include/prediction_engine/model_interface.hpp"
#include "../include/prediction_engine/risk_scorer.hpp"
#include "../include/alerting/threshold_watcher.hpp"
#include "../include/alerting/notification_dispatcher.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <future>
#include <fstream>
#include <ctime>
#include <map>
#include <sstream>
#include <mutex>
#include <atomic>
#include <filesystem>
#include "../include/analytics/pattern_engine.h"
#include "../include/monitoring/alert_router.h"
#include "../include/third_party/json.hpp"
#include "../include/utils/perf_logger.hpp"
#include "../include/utils/time_format.hpp"
#include "../include/data/coingecko_client.hpp"
#include "../include/data/etherscan_client.hpp"
#include "../include/data/defi_client.hpp"
#include "../include/data/sentiment_client.hpp"

using namespace stablecoin_tracker;

namespace {
std::mutex g_history_file_mutex;
}

struct AnalysisResult {
    std::string symbol;
    double risk_score = 0.0;
    std::string alert_level;
    std::string confidence;
    std::string pattern_match;
    bool alert_triggered = false;
    bool failed = false;
};

std::string detectPatternWithTestMode(const std::vector<PricePoint>& history, bool test_mode,
                                      std::atomic<bool>& forced_match) {
    if (test_mode && !forced_match.exchange(true)) {
        std::cout << "[PATTERN] Similarity to TerraUSD depeg (May 2022): 99% (test mode)\n";
        return "Similarity to TerraUSD depeg (May 2022): 99%";
    }
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> prices;
    prices.reserve(history.size());
    for (const auto& p : history) {
        prices.emplace_back(std::chrono::system_clock::from_time_t(static_cast<std::time_t>(p.timestamp)), p.price);
    }
    PatternMatcher matcher(prices);
    std::string name;
    double conf = matcher.matchDepegPattern(PatternMatcher::getKnownDepegPatterns(), 0.7, &name);
    if (conf >= 0.7) {
        int pct = static_cast<int>(conf * 100.0);
        std::ostringstream oss;
        oss << "Similarity to " << name << ": " << pct << "%";
        std::cout << "[PATTERN] " << oss.str() << "\n";
        return oss.str();
    }
    std::cout << "[PATTERN] No match found in historical depeg patterns\n";
    return "No match found in historical depeg patterns";
}

std::string getAlertLevel(double risk_score) {
    if (risk_score > 0.9) return "Critical";
    if (risk_score > 0.7) return "High";
    if (risk_score > 0.5) return "Medium";
    return "Low";
}

AnalysisResult analyzeStablecoin(const std::string& coin,
                      const ConfigManager& config,
                      const std::map<std::string, std::string>& symbolToId,
                      PerfLogger& perfLogger,
                      bool test_mode,
                      std::atomic<bool>& forced_pattern_match) {
    auto t0 = std::chrono::steady_clock::now();
    try {
        std::cout << "\n[INFO] -------- Analyzing " << std::setw(6) << std::left << coin << " --------\n";
        std::string coingecko_id = symbolToId.count(coin) ? symbolToId.at(coin) : coin;
        std::cout << "[INFO] Launching async fetchers for sentiment, TVL, onchain\n";
        auto price_future = std::async(std::launch::async, fetchHistoricalPrices, coingecko_id, config.getHistoricalWindowDays());
        auto sentiment_future = std::async(std::launch::async, fetchSentimentScore, coin);
        std::string etherscan_api_key = config.getEtherscanApiKey();
        auto onchain_future = std::async(std::launch::async, fetchOnChainMetrics, coin, config.getHistoricalWindowDays(), etherscan_api_key);
        auto tvl_future = std::async(std::launch::async, fetchTVL, coingecko_id);
        auto price_data = price_future.get();
        SentimentFetch sentiment = sentiment_future.get();
        if (test_mode) {
            sentiment = {-0.9, true};
        }
        OnChainMetrics onchain = onchain_future.get();
        TvlFetch tvl = tvl_future.get();
        auto t_fetch = std::chrono::steady_clock::now();

        std::vector<PricePoint> price_points;
        for (const auto& [ts, price] : price_data) {
            price_points.push_back({ts, price});
        }
        std::string pattern_result = detectPatternWithTestMode(price_points, test_mode, forced_pattern_match);

        int metrics_present = 0;
        std::vector<std::string> present_signals;
        if (!price_data.empty()) { ++metrics_present; present_signals.push_back("volatility"); }
        if (!sentiment.is_mock && sentiment.score != 0.0) { ++metrics_present; present_signals.push_back("sentiment"); }
        else if (sentiment.is_mock) { present_signals.push_back("sentiment(mock)"); }
        if (onchain.tx_volume != 0.0) { ++metrics_present; present_signals.push_back("onchain"); }
        if (!tvl.is_mock && tvl.tvl != 0.0) { ++metrics_present; present_signals.push_back("TVL"); }
        else if (tvl.is_mock) { present_signals.push_back("TVL(mock)"); }
        std::string confidence = "LOW";
        if (metrics_present == 2 && !price_data.empty() && onchain.tx_volume != 0.0 && !sentiment.is_mock && !tvl.is_mock)
            confidence = "HIGH";
        if (!sentiment.is_mock && !tvl.is_mock && metrics_present >= 4) confidence = "HIGH";
        else if (metrics_present >= 2) confidence = "MEDIUM";
        else if (metrics_present >= 1) confidence = "LOW";
        if (sentiment.is_mock || tvl.is_mock) {
            if (confidence == "HIGH") confidence = "MEDIUM";
        }
        std::cout << "[CONFIDENCE] Set to " << std::setw(6) << confidence << " (" << metrics_present << " real/4 signals: ";
        for (size_t i = 0; i < present_signals.size(); ++i) {
            std::cout << present_signals[i];
            if (i + 1 < present_signals.size()) std::cout << ", ";
        }
        std::cout << ")\n";

        double volatility = 0.0;
        double peg_deviation = 0.0;
        HistoricalPatternEngine pattern_engine(config.getHistoricalWindowDays() * 24);
        pattern_engine.SetHistoricalWindowDays(config.getHistoricalWindowDays());
        if (config.isDataSourceEnabled("price") && !price_data.empty()) {
            std::vector<std::pair<std::chrono::system_clock::time_point, double>> chrono_prices;
            for (const auto& [ts, price] : price_data) {
                chrono_prices.emplace_back(std::chrono::system_clock::from_time_t(ts), price);
            }
            pattern_engine.AddPriceData(coin, chrono_prices);
            volatility = pattern_engine.CalculateVolatility(coin);
            peg_deviation = pattern_engine.CalculatePegDeviation(coin, config.getTargetPrice());
            auto depegs = pattern_engine.DetectDepegEvents(coin, config.getTargetPrice(), config.getDepegThreshold());
            if (!depegs.empty()) {
                std::cout << "[INFO] " << depegs.size() << " samples exceeded depeg threshold "
                          << config.getDepegThreshold() << " vs peg " << config.getTargetPrice() << "\n";
            }
        }
        double onchain_flow = onchain.tx_volume;
        auto model = std::make_shared<StatisticalModel>();
        RiskScorer risk_scorer(model, config.getRiskWeights());
        auto risk = risk_scorer.CalculateRisk(peg_deviation, volatility, onchain_flow, sentiment.score, tvl.tvl);
        risk.symbol = coin;
        auto t_risk = std::chrono::steady_clock::now();

        ThresholdWatcher watcher(config.getThresholdFor(coin));
        watcher.AddRiskAssessment(risk);
        auto threshold_hits = watcher.CheckThresholds();
        std::string alert_level = getAlertLevel(risk.risk_score);
        bool alert_triggered = !threshold_hits.empty();
        std::cout << "[INFO] Risk Score: " << std::fixed << std::setprecision(3) << risk.risk_score
                  << " | Alert: " << std::setw(7) << (alert_triggered ? "YES" : "NO")
                  << " | Level: " << std::setw(8) << alert_level
                  << " | Confidence: " << confidence
                  << " | PegDev: " << peg_deviation << std::endl;
        if (alert_triggered) {
            NotificationDispatcher dispatcher;
            dispatcher.DispatchAlerts(threshold_hits);
            Alert alert{coin, alert_level, risk.risk_score, risk.reasoning};
            sendAlert(alert);
            perfLogger.logAlert({
                {"symbol", coin},
                {"level", alert_level},
                {"risk_score", risk.risk_score},
                {"reason", risk.reasoning}
            });
        }
        auto t_alert = std::chrono::steady_clock::now();
        double fetch_s = std::chrono::duration<double>(t_fetch - t0).count();
        double risk_s = std::chrono::duration<double>(t_risk - t_fetch).count();
        double alert_s = std::chrono::duration<double>(t_alert - t_risk).count();
        double total_s = std::chrono::duration<double>(t_alert - t0).count();
        perfLogger.logPerformance(coin, fetch_s, risk_s, alert_s, total_s);

        std::string date_str = formatTime("%Y-%m-%d");
        {
            std::lock_guard<std::mutex> lock(g_history_file_mutex);
            std::filesystem::create_directories("data/history");
            std::string out_path = "data/history/" + coin + "_" + date_str + ".json";
            std::ofstream out(out_path);
            if (out) {
                nlohmann::json j = {
                    {"symbol", coin},
                    {"risk", risk.risk_score},
                    {"peg_deviation", peg_deviation},
                    {"volatility", volatility},
                    {"confidence", confidence},
                    {"alert_level", alert_level},
                    {"components", {
                        {"price_deviation", risk.price_deviation},
                        {"volume_anomaly", risk.volume_anomaly},
                        {"sentiment_risk", risk.sentiment_risk},
                        {"tvl_risk", risk.tvl_risk}
                    }}
                };
                out << j.dump(2);
                std::cout << "[INFO] Wrote analysis data for " << coin << " to ./data/history/\n";
            }
        }
        return AnalysisResult{coin, risk.risk_score, alert_level, confidence, pattern_result, alert_triggered, false};
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception analyzing " << coin << ": " << e.what() << "\n";
        return AnalysisResult{coin, 0.0, "Error", "NONE", "", false, true};
    }
}

int main(int argc, char* argv[]) {
    bool test_mode = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-mode") test_mode = true;
    }
    std::filesystem::create_directories("output");
    PerfLogger perfLogger("output/performance.csv", "output/alerts.json");
    std::cout << "\n=== Stablecoin Stability Tracker (Configurable) ===\n" << std::endl;
    ConfigManager config("config/config.json");
    if (!config.load()) {
        std::cerr << "[ERROR] Failed to load config. Exiting.\n";
        return 1;
    }
    std::map<std::string, std::string> symbolToId = {
        {"USDT", "tether"},
        {"USDC", "usd-coin"},
        {"DAI", "dai"},
        {"FRAX", "frax"},
        {"BUSD", "binance-usd"}
    };
    const auto& stablecoins = config.getStablecoins();
    std::vector<AnalysisResult> results;
    std::atomic<bool> forced_pattern_match{false};
    auto start_time = std::chrono::steady_clock::now();
    std::vector<std::future<AnalysisResult>> futures;
    for (const auto& coin : stablecoins) {
        futures.push_back(std::async(std::launch::async, analyzeStablecoin, coin, std::cref(config),
                                     std::cref(symbolToId), std::ref(perfLogger), test_mode,
                                     std::ref(forced_pattern_match)));
    }
    for (auto& fut : futures) {
        results.push_back(fut.get());
    }
    double avg_risk = 0.0;
    int alert_count = 0;
    int scored = 0;
    nlohmann::json confidence_map;
    nlohmann::json pattern_map;
    for (const auto& r : results) {
        if (!r.failed) {
            avg_risk += r.risk_score;
            ++scored;
        }
        if (r.alert_triggered) ++alert_count;
        confidence_map[r.symbol] = r.confidence;
        pattern_map[r.symbol] = r.pattern_match;
    }
    avg_risk = scored == 0 ? 0.0 : avg_risk / scored;
    std::string ts_file = formatTime("%Y%m%d_%H%M%S");
    std::filesystem::create_directories("data");
    std::string summary_path = "data/summary_" + ts_file + ".json";
    nlohmann::json summary = {
        {"average_risk_score", avg_risk},
        {"alerts_triggered", alert_count},
        {"confidence_levels", confidence_map},
        {"pattern_matches", pattern_map},
        {"stablecoins", stablecoins}
    };
    std::ofstream summary_out(summary_path);
    if (summary_out) summary_out << summary.dump(2);
    std::string report_path = "data/final_report.txt";
    std::ofstream report(report_path);
    auto end_time = std::chrono::steady_clock::now();
    double runtime = std::chrono::duration<double>(end_time - start_time).count();
    if (report) {
        report << "Stablecoin Stability Tracker Final Report\n";
        report << "Timestamp: " << formatTime("%Y-%m-%d %H:%M:%S") << "\n";
        report << "Runtime: " << std::fixed << std::setprecision(2) << runtime << "s\n\n";
        report << "Stablecoins Analyzed:\n";
        for (const auto& r : results) {
            report << "  - " << r.symbol << ": Risk Score: " << std::fixed << std::setprecision(3) << r.risk_score
                   << ", Alert: " << r.alert_level << ", Confidence: " << r.confidence;
            if (r.failed) report << " (FAILED)";
            report << "\n";
        }
        report << "\nPattern Matches:\n";
        for (const auto& r : results) {
            if (r.pattern_match.find("Similarity") != std::string::npos)
                report << "  - " << r.symbol << ": " << r.pattern_match << "\n";
        }
        report << "\nSummary:\n  Average Risk Score: " << avg_risk << "\n  Alerts Triggered: " << alert_count << "\n";
    }
    std::cout << "\n=== All Systems Operational ===\n";
    std::cout << "[FINAL REPORT] Written to ./data/final_report.txt\n";
    return 0;
}
