// test_summary_parser.cpp
// Unit test for summary JSON report in Stablecoin Stability Tracker
#include "../include/third_party/json.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string latest_summary;
    // Find latest summary file in ./data/
    std::string prefix = "data/summary_";
    std::string found;
    for (int i = 0; i < 100; ++i) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%02d", i);
        std::string path = prefix + buf + ".json";
        std::ifstream f(path);
        if (f) found = path;
    }
    if (found.empty()) {
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