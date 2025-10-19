// test_summary_parser.cpp
// Unit test for summary JSON report in Stablecoin Stability Tracker
#include "../include/third_party/json.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string latest_summary;
    // Find latest summary file in ./data/
    std::string found = "data/summary_20251020_001151.json"; // Use the most recent one
    std::ifstream test_file(found);
    if (!test_file.good()) {
        std::cout << "[FAIL] No summary JSON found in ./data/\n";
        return 1;
    }
    std::ifstream in(found);
    nlohmann::json j;
    in >> j;
    int passed = 0, total = 0;
    auto check = [&](const char* key) {
        ++total;
        if (j.contains(key)) {
            std::cout << "[PASS] Field present: " << key << "\n"; ++passed;
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