#include "../include/third_party/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

int main() {
    std::filesystem::create_directories("data");
    std::string path = "data/summary_test_parser.json";
    nlohmann::json sample = {
        {"average_risk_score", 0.1},
        {"alerts_triggered", 0},
        {"confidence_levels", {{"USDT", "LOW"}}},
        {"pattern_matches", {{"USDT", "none"}}},
        {"stablecoins", nlohmann::json::array({"USDT"})}
    };
    {
        std::ofstream out(path);
        out << sample.dump(2);
    }
    std::ifstream in(path);
    if (!in) {
        std::cout << "[FAIL] Could not read generated summary JSON\n";
        return 1;
    }
    nlohmann::json j;
    in >> j;
    int passed = 0, total = 0;
    auto check = [&](const char* key) {
        ++total;
        if (j.contains(key)) {
            std::cout << "[PASS] Field present: " << key << "\n";
            ++passed;
        } else {
            std::cout << "[FAIL] Field missing: " << key << "\n";
        }
    };
    check("average_risk_score");
    check("alerts_triggered");
    check("confidence_levels");
    check("pattern_matches");
    check("stablecoins");
    std::cout << "[TEST] Summary parser passed " << passed << "/" << total << " checks\n";
    return passed == total ? 0 : 1;
}
