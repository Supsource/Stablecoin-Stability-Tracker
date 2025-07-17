#pragma once
#include <string>

namespace stablecoin_tracker {
struct Alert {
    std::string symbol;
    std::string level;
    double risk_score;
};
void sendAlert(const Alert& alert);
} 