#include "thread_pool.h"
#include "logger.h"

ThreadPool::ThreadPool(size_t num_threads) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool() {
    stopping = true;
    cv.notify_all();
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }
}

bool ThreadPool::enqueue(std::function<void()> job) {
    if (stopping) return false;
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        jobs.push(std::move(job));
    }
    cv.notify_one();
    return true;
}

void ThreadPool::worker_loop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            cv.wait(lock, [this] { return !jobs.empty() || stopping; });
            if (stopping && jobs.empty()) return;
            job = std::move(jobs.front());
            jobs.pop();
        }
        // Run outside the lock — other threads can pick up jobs while we work
        try {
            job();
        } catch (const std::exception& e) {
            LOG_ERROR(std::string("[worker] unhandled exception: ") + e.what());
        } catch (...) {
            LOG_ERROR("[worker] unknown exception");
        }
    }
}