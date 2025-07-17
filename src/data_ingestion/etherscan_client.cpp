#include "../../include/data/etherscan_client.hpp"
#include <iostream>
#include <cstdlib>
#include <map>
#include <curl/curl.h>
#include "../../include/third_party/json.hpp"
#include <chrono>
#include <set>

namespace stablecoin_tracker {
namespace {
// Real Ethereum contract addresses
const std::map<std::string, std::string> symbolToAddress = {
    {"USDT", "0xdAC17F958D2ee523a2206206994597C13D831ec7"},
    {"USDC", "0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48"},
    {"DAI",  "0x6B175474E89094C44Da98b954EedeAC495271d0F"}
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
}

OnChainMetrics fetchOnChainMetrics(const std::string& symbol, int days, const std::string& etherscan_api_key) {
    // Get contract address for the symbol
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
    // Calculate start time (30 days ago)
    auto now = std::chrono::system_clock::now();
    auto start_time = now - std::chrono::hours(24 * days);
    std::time_t start_ts = std::chrono::system_clock::to_time_t(start_time);
    // Etherscan API
    std::string url = "https://api.etherscan.io/api?module=account&action=tokentx&contractaddress=" + contractAddress + "&startblock=0&endblock=99999999&sort=desc&apikey=" + etherscan_api_key;
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[ERROR] Failed to initialize CURL\n";
        return {};
    }
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        std::cerr << "[ERROR] CURL error: " << curl_easy_strerror(res) << "\n";
        return {};
    }
    nlohmann::json jsonResponse;
    try {
        jsonResponse = nlohmann::json::parse(response);
    } catch (...) {
        std::cerr << "[ERROR] Failed to parse Etherscan response\n";
        return {};
    }
    if (!jsonResponse.contains("result") || !jsonResponse["result"].is_array()) {
        std::cerr << "[ERROR] Etherscan response missing 'result' array\n";
        return {};
    }
    double tx_volume_wei = 0.0;
    std::set<std::string> unique_wallets;
    for (const auto& tx : jsonResponse["result"]) {
        // Only include transfers within the last N days
        if (!tx.contains("timeStamp") || !tx.contains("value") || !tx.contains("from") || !tx.contains("to")) continue;
        std::time_t tx_time = std::stoll(tx["timeStamp"].get<std::string>());
        if (tx_time < start_ts) continue;
        double value = 0.0;
        try { value = std::stod(tx["value"].get<std::string>()); } catch (...) { continue; }
        tx_volume_wei += value;
        unique_wallets.insert(tx["from"].get<std::string>());
        unique_wallets.insert(tx["to"].get<std::string>());
    }
    int active_wallet_count = unique_wallets.size();
    // Convert tx_volume from wei to ETH (1 ETH = 1e18 wei)
    double tx_volume_eth = tx_volume_wei / 1e18;
    std::cout << "[INFO] On-chain tx volume (ETH): " << tx_volume_eth << ", active wallets: " << active_wallet_count << "\n";
    return {tx_volume_eth, active_wallet_count};
}
} 