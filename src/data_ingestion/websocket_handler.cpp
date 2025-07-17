#include "../../include/data/websocket_handler.hpp"
#include <iostream>

namespace stablecoin_tracker {
WebSocketHandler::WebSocketHandler(const std::string& url) : url_(url), connected_(false) {}

bool WebSocketHandler::Connect() {
    // Simulate connection
    connected_ = true;
    std::cout << "WebSocket connected to " << url_ << "\n";
    return true;
}

void WebSocketHandler::Disconnect() {
    // Simulate Disconnect
    connected_ = false;
    std::cout << "WebSocket disconnected from " << url_ << "\n";
}

bool WebSocketHandler::IsConnected() const {
    return connected_;
}

void WebSocketHandler::SimulateReceive(const std::string& msg) {
    // simulate receiving a message
    std::cout << "Received message: " << msg << "\n";
}
}
