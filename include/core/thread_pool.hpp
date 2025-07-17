#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace stablecoin_tracker {
class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    void Enqueue(std::function<void()> task);
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    std::atomic<bool> stop_;
    void Worker();
};
}
