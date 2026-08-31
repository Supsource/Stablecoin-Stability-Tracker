#include "../../include/data/coingecko_client.hpp"
#include "../../include/utils/curl_raii.hpp"
#include <curl/curl.h>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <cctype>
#include "../../include/third_party/json.hpp"
#include <filesystem>
#include <fstream>

namespace stablecoin_tracker {
namespace {
std::mutex g_coingecko_mutex;

constexpr int CACHE_EXPIRY_SECONDS = 12 * 60 * 60;

std::string sanitizeId(const std::string& id) {
    std::string out;
    out.reserve(id.size());
    for (char c : id) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_') {
            out.push_back(c);
        }
    }
    return out.empty() ? "unknown" : out;
}

std::string getCachePath(const std::string& coinGeckoId, int days) {
    return ".cache/" + sanitizeId(coinGeckoId) + "_" + std::to_string(days) + "d.json";
}

bool isCacheFresh(const std::string& path) {
    namespace fs = std::filesystem;
    if (!fs::exists(path)) return false;
    auto last_write = fs::last_write_time(path);
    auto now = fs::file_time_type::clock::now();
    auto age = std::chrono::duration_cast<std::chrono::seconds>(now - last_write).count();
    return age < CACHE_EXPIRY_SECONDS;
}

bool readCache(const std::string& path, nlohmann::json& outJson) {
    std::ifstream in(path);
    if (!in) return false;
    try {
        in >> outJson;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void writeCache(const std::string& path, const nlohmann::json& json) {
    std::filesystem::create_directories(".cache");
    std::ofstream out(path);
    if (out) {
        out << json.dump();
    }
}

bool isRateLimitError(long http_code, const nlohmann::json& jsonResponse) {
    if (http_code == 429) return true;
    if (jsonResponse.contains("status") && jsonResponse["status"].contains("error_code") &&
        jsonResponse["status"]["error_code"] == 429) {
        return true;
    }
    return false;
}

std::vector<std::pair<long long, double>> pricesFromJson(const nlohmann::json& json) {
    std::vector<std::pair<long long, double>> result;
    if (!json.contains("prices") || !json["prices"].is_array()) return result;
    for (const auto& entry : json["prices"]) {
        if (entry.is_array() && entry.size() == 2) {
            long long timestamp_ms = entry[0];
            double price = entry[1];
            result.push_back({ timestamp_ms / 1000, price });
        }
    }
    return result;
}
}

std::vector<std::pair<long long, double>> fetchHistoricalPrices(
    const std::string& coinGeckoId,
    int days
) {
    std::lock_guard<std::mutex> throttle(g_coingecko_mutex);
    std::vector<std::pair<long long, double>> result;
    std::string cachePath = getCachePath(coinGeckoId, days);
    nlohmann::json cachedJson;
    if (isCacheFresh(cachePath) && readCache(cachePath, cachedJson)) {
        result = pricesFromJson(cachedJson);
        if (!result.empty()) {
            std::cout << "[INFO] [CACHE] Used cached price data for " << coinGeckoId << "\n";
            return result;
        }
        std::cerr << "[WARN] [CACHE] Cache for " << coinGeckoId << " missing 'prices' field. Ignoring cache.\n";
    }
    std::string url = "https://api.coingecko.com/api/v3/coins/" + coinGeckoId +
                      "/market_chart?vs_currency=usd&days=" + std::to_string(days);
    int retries = 3;
    int waitTime = 3;
    for (int attempt = 1; attempt <= retries; ++attempt) {
        CurlEasy curl;
        if (!curl) {
            std::cerr << "[ERROR] Failed to initialize CURL" << std::endl;
            return result;
        }
        std::string raw_response_string;
        curl.setCommonOptions(url, raw_response_string);
        CURLcode res = curl_easy_perform(curl.get());
        long http_code = curl.responseCode();
        nlohmann::json jsonResponse;
        bool parse_error = false;
        try {
            jsonResponse = nlohmann::json::parse(raw_response_string);
        } catch (const std::exception&) {
            parse_error = true;
        }
        if (res == CURLE_OK && http_code == 200 && !isRateLimitError(http_code, jsonResponse) && !parse_error) {
            if (jsonResponse.contains("error")) {
                std::cerr << "[ERROR] CoinGecko API returned error: " << jsonResponse["error"] << "\n";
                return {};
            }
            result = pricesFromJson(jsonResponse);
            if (result.empty()) {
                std::cerr << "[ERROR] No 'prices' field in response for " << coinGeckoId << "\n";
                return {};
            }
            std::cout << "[INFO] Fetched " << result.size() << " price points for " << coinGeckoId << "\n";
            writeCache(cachePath, jsonResponse);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return result;
        } else if (isRateLimitError(http_code, jsonResponse) || http_code == 429) {
            std::cerr << "[WARN] Rate limit hit for " << coinGeckoId << ", retrying in " << waitTime << "s...\n";
            std::this_thread::sleep_for(std::chrono::seconds(waitTime));
            waitTime *= 2;
        } else if (http_code >= 500 && attempt < retries) {
            std::cerr << "[WARN] CoinGecko HTTP " << http_code << " for " << coinGeckoId
                      << ", retrying in " << waitTime << "s...\n";
            std::this_thread::sleep_for(std::chrono::seconds(waitTime));
            waitTime *= 2;
        } else if (parse_error) {
            std::cerr << "[ERROR] Failed to parse JSON for " << coinGeckoId << "\n";
            break;
        } else {
            std::cerr << "[ERROR] Failed to fetch historical prices for " << coinGeckoId
                      << " HTTP " << http_code << ": " << curl_easy_strerror(res) << "\n";
            break;
        }
    }
    std::cerr << "[ERROR] Failed to fetch data for " << coinGeckoId
              << " after retries. Using stale cache if available.\n";
    if (readCache(cachePath, cachedJson)) {
        result = pricesFromJson(cachedJson);
        if (!result.empty()) {
            std::cerr << "[WARN] [CACHE] Used stale cache for " << coinGeckoId << "\n";
            return result;
        }
    }
    return {};
}
}
