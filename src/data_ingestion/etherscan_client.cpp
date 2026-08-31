#include "../../include/data/etherscan_client.hpp"
#include "../../include/utils/curl_raii.hpp"
#include <iostream>
#include <map>
#include <cmath>
#include <curl/curl.h>
#include "../../include/third_party/json.hpp"
#include <chrono>
#include <ctime>
#include <set>

namespace stablecoin_tracker {
namespace {
const std::map<std::string, std::string> symbolToAddress = {
    {"USDT", "0xdAC17F958D2ee523a2206206994597C13D831ec7"},
    {"USDC", "0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48"},
    {"DAI",  "0x6B175474E89094C44Da98b954EedeAC495271d0F"},
    {"FRAX", "0x853d955aCEc86633000D26774607eE3c330641eC"},
    {"BUSD", "0x4Fabb145d64652a948d72533023f6E7A623C7C53"}
};

constexpr double kLargeTransferTokens = 1'000'000.0;
}

int tokenDecimalsForSymbol(const std::string& symbol) {
    if (symbol == "USDT" || symbol == "USDC") return 6;
    return 18; // DAI, FRAX, BUSD
}

double rawAmountToTokens(const std::string& raw_value, int decimals) {
    double value = 0.0;
    try {
        value = std::stod(raw_value);
    } catch (...) {
        return 0.0;
    }
    return value / std::pow(10.0, decimals);
}

OnChainMetrics fetchOnChainMetrics(const std::string& symbol, int days, const std::string& etherscan_api_key) {
    auto it = symbolToAddress.find(symbol);
    if (it == symbolToAddress.end()) {
        std::cerr << "[ERROR] No contract address for symbol: " << symbol << "\n";
        return {};
    }
    std::string contractAddress = it->second;
    if (etherscan_api_key.empty()) {
        std::cerr << "[ERROR] ETHERSCAN_API_KEY not set in config. Skipping on-chain fetch for " << symbol << "\n";
        return {};
    }
    std::cout << "[INFO] Fetching on-chain metrics for " << symbol << " (" << contractAddress << ") over " << days << " days\n";
    auto now = std::chrono::system_clock::now();
    auto start_time = now - std::chrono::hours(24 * days);
    std::time_t start_ts = std::chrono::system_clock::to_time_t(start_time);
    std::string url = "https://api.etherscan.io/api?module=account&action=tokentx&contractaddress=" +
                      contractAddress + "&startblock=0&endblock=99999999&sort=desc&apikey=" + etherscan_api_key;
    CurlEasy curl;
    if (!curl) {
        std::cerr << "[ERROR] Failed to initialize CURL\n";
        return {};
    }
    std::string response;
    curl.setCommonOptions(url, response);
    CURLcode res = curl_easy_perform(curl.get());
    long http_code = curl.responseCode();
    if (res != CURLE_OK) {
        std::cerr << "[ERROR] CURL error: " << curl_easy_strerror(res) << "\n";
        return {};
    }
    if (http_code != 200) {
        std::cerr << "[ERROR] Etherscan HTTP " << http_code << "\n";
        return {};
    }
    nlohmann::json jsonResponse;
    try {
        jsonResponse = nlohmann::json::parse(response);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to parse Etherscan response: " << e.what() << "\n";
        return {};
    }
    if (jsonResponse.contains("status") && jsonResponse["status"].is_string() &&
        jsonResponse["status"].get<std::string>() == "0") {
        std::string msg = jsonResponse.value("message", "unknown error");
        std::cerr << "[ERROR] Etherscan API error: " << msg << "\n";
        return {};
    }
    if (!jsonResponse.contains("result") || !jsonResponse["result"].is_array()) {
        std::cerr << "[ERROR] Etherscan response missing 'result' array\n";
        return {};
    }
    int decimals = tokenDecimalsForSymbol(symbol);
    double tx_volume = 0.0;
    int large_transfers = 0;
    std::set<std::string> unique_wallets;
    for (const auto& tx : jsonResponse["result"]) {
        if (!tx.contains("timeStamp") || !tx.contains("value") || !tx.contains("from") || !tx.contains("to")) continue;
        std::time_t tx_time = 0;
        try {
            tx_time = static_cast<std::time_t>(std::stoll(tx["timeStamp"].get<std::string>()));
        } catch (...) {
            continue;
        }
        if (tx_time < start_ts) continue;
        double tokens = rawAmountToTokens(tx["value"].get<std::string>(), decimals);
        tx_volume += tokens;
        if (tokens >= kLargeTransferTokens) {
            ++large_transfers;
        }
        unique_wallets.insert(tx["from"].get<std::string>());
        unique_wallets.insert(tx["to"].get<std::string>());
    }
    int active_wallet_count = static_cast<int>(unique_wallets.size());
    std::cout << "[INFO] On-chain tx volume (tokens): " << tx_volume
              << ", large transfers: " << large_transfers
              << ", active wallets: " << active_wallet_count << "\n";
    return {tx_volume, large_transfers, active_wallet_count};
}
}
