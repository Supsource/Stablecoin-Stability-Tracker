#pragma once
#include <vector>
#include <utility>
#include <chrono>

namespace stablecoin_tracker {
template<typename T>
class TimeSeries {
public:
    void AddDataPoint(std::chrono::system_clock::time_point ts, const T& value) {
        data_.emplace_back(ts, value);
    }
    const std::vector<std::pair<std::chrono::system_clock::time_point, T>>& GetData() const {
        return data_;
    }
private:
    std::vector<std::pair<std::chrono::system_clock::time_point, T>> data_;
};
}
