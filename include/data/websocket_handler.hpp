#pragma once
#include <string>
#include <atomic>

namespace stablecoin_tracker {
class WebSocketHandler {
public:
    WebSocketHandler(const std::string& url);
    bool Connect();
    void Disconnect();
    bool IsConnected() const;
    void SimulateReceive(const std::string& msg);
private:
    std::string url_;
    std::atomic<bool> connected_;
};
}
