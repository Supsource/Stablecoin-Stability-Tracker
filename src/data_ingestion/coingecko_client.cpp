#include "../../include/data/coingecko_client.hpp"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <map>
#include <thread>
#include <chrono>
#include "../../include/third_party/json.hpp"
#include <filesystem>
#include <fstream>

namespace stablecoin_tracker {
namespace {
// maps stablecoin symbols to CoinGecko IDs
const std::map<std::string, std::string> symbolToId = {
    {"USDT", "tether"},
    {"USDC", "usd-coin"},
    {"DAI", "dai"},
    {"FRAX", "frax"},
    {"BUSD", "binance-usd"}
};

constexpr int CACHE_EXPIRY_SECONDS = 12 * 60 * 60; // equals to 12 hours

std::string getCachePath(const std::string& coinGeckoId) {
    return ".cache/" + coinGeckoId + ".json";
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
    } catch (...) {
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

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Helper to check for rate limit error in JSON or HTTP code
bool isRateLimitError(long http_code, const nlohmann::json& jsonResponse) {
    if (http_code == 429) return true;
    if (jsonResponse.contains("status") && jsonResponse["status"].contains("error_code") && jsonResponse["status"]["error_code"] == 429) return true;
    return false;
}
}

std::vector<std::pair<long long, double>> fetchHistoricalPrices(
    const std::string& coinGeckoId,
    int days
) {
    std::vector<std::pair<long long, double>> result;
    std::string cachePath = getCachePath(coinGeckoId);
    nlohmann::json cachedJson;
    if (isCacheFresh(cachePath) && readCache(cachePath, cachedJson)) {
        if (cachedJson.contains("prices")) {
            for (const auto& entry : cachedJson["prices"]) {
                if (entry.is_array() && entry.size() == 2) {
                    long long timestamp_ms = entry[0];
                    double price = entry[1];
                    result.push_back({ timestamp_ms / 1000, price });
                }
            }
            std::cout << "[INFO] [CACHE] Used cached price data for " << coinGeckoId << "\n";
            return result;
        } else {
            std::cerr << "[WARN] [CACHE] Cache for " << coinGeckoId << " missing 'prices' field. Ignoring cache.\n";
        }
    }
    std::string url = "https://api.coingecko.com/api/v3/coins/" + coinGeckoId + "/market_chart?vs_currency=usd&days=" + std::to_string(days);
    int retries = 3;
    int waitTime = 3;
    for (int attempt = 1; attempt <= retries; ++attempt) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "[ERROR] Failed to initialize CURL" << std::endl;
            return result;
        }
        std::string raw_response_string;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raw_response_string);
        long http_code = 0;
        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        curl_easy_cleanup(curl);
        nlohmann::json jsonResponse;
        bool parse_error = false;
        try {
            jsonResponse = nlohmann::json::parse(raw_response_string);
        } catch (const std::exception& e) {
            parse_error = true;
        }
        if (res == CURLE_OK && !isRateLimitError(http_code, jsonResponse) && !parse_error) {
            if (jsonResponse.contains("error")) {
                std::cerr << "[ERROR] CoinGecko API returned error: " << jsonResponse["error"] << "\n";
                return {};
            }
            if (!jsonResponse.contains("prices")) {
                std::cerr << "[ERROR] No 'prices' field in response for " << coinGeckoId << "\n";
                std::cerr << "Full response: " << raw_response_string << "\n";
                return {};
            }
            for (const auto& entry : jsonResponse["prices"]) {
                if (entry.is_array() && entry.size() == 2) {
                    long long timestamp_ms = entry[0];
                    double price = entry[1];
                    result.push_back({ timestamp_ms / 1000, price });
                }
            }
            std::cout << "[INFO] Fetched " << result.size() << " price points for " << coinGeckoId << "\n";
            writeCache(cachePath, jsonResponse);
            std::this_thread::sleep_for(std::chrono::seconds(2)); // Preventive throttling
            return result;
        } else if (isRateLimitError(http_code, jsonResponse)) {
            std::cerr << "[WARN] Rate limit hit for " << coinGeckoId << ", retrying in " << waitTime << "s...\n";
            std::this_thread::sleep_for(std::chrono::seconds(waitTime));
            waitTime *= 2;
        } else if (parse_error) {
            std::cerr << "[ERROR] Failed to parse JSON for " << coinGeckoId << "\n";
            std::cerr << "Raw response: " << raw_response_string << "\n";
            return {};
        } else {
            std::cerr << "[ERROR] Failed to fetch historical prices for " << coinGeckoId << ": " << curl_easy_strerror(res) << "\n";
            std::cerr << "Raw response: " << raw_response_string << "\n";
            return {};
        }
    }
    std::cerr << "[ERROR] Failed to fetch data for " << coinGeckoId << " after " << retries << " retries. Skipping and using stale cache if available.\n";
    if (readCache(cachePath, cachedJson) && cachedJson.contains("prices")) {
        for (const auto& entry : cachedJson["prices"]) {
            if (entry.is_array() && entry.size() == 2) {
                long long timestamp_ms = entry[0];
                double price = entry[1];
                result.push_back({ timestamp_ms / 1000, price });
            }
        }
        std::cerr << "[WARN] [CACHE] Used stale cache for " << coinGeckoId << "\n";
        return result;
    }
    return {};
}
} 