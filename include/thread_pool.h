#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // Queue a job. Returns false if the pool is already shutting down.
    bool enqueue(std::function<void() > job);

    size_t thread_count() const { return workers.size(); }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;

    std::mutex queue_mutex;
    std::condition_variable cv;
    std::atomic<bool> stopping{false};

    void worker_loop();
};

#endif