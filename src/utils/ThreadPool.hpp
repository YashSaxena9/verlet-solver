#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace mt {

class ThreadPool {
public:
    const uint32_t threadCount;

    explicit ThreadPool(size_t threadCount);
    ~ThreadPool();

    // Submit a single task
    void addTask(std::function<void()> task);

    // Dispatch a range of work
    template <typename Callback>
    void dispatch(size_t count, Callback callback);

    void wait();

private:
    void workerLoop();

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::atomic<bool> m_stop = { false };
    std::atomic<size_t> m_pending = { 0 };
};

// Constructor: start worker threads
inline ThreadPool::ThreadPool(size_t threadCount): threadCount(threadCount) {
    for (size_t i = 0; i < threadCount; i++) {
        m_workers.emplace_back(&ThreadPool::workerLoop, this);
    }
}

// Destructor: stop all threads
inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_cv.notify_all();
    for (auto& t : m_workers) {
        t.join();
    }
}

// Adds a task to the queue
inline void ThreadPool::addTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(move(task));
        m_pending++;
    }
    m_cv.notify_one();
}

// Dispatches work across threads in batches
template <typename Callback>
inline void ThreadPool::dispatch(size_t count, Callback callback) {
    wait();
    const size_t threadCount = m_workers.size();
    const size_t batch = count / threadCount;
    const size_t extra = count % threadCount;

    size_t start = 0;
    for (size_t i = 0; i < threadCount; i++) {
        size_t end = start + batch + (i < extra ? 1 : 0);
        if (start >= end) {
            // Avoid empty batches
            break;
        }

        addTask([=]() {
            callback(start, end);
        });

        start = end;
    }

    wait();
}

// Waits for all tasks to complete
inline void ThreadPool::wait() {
    // while (m_pending > 0) {
    //     std::this_thread::yield();
    // }
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [&] { return m_pending == 0; });
}

// Worker thread function
inline void ThreadPool::workerLoop() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [&]() {
                return m_stop || !m_tasks.empty();
            });

            if (m_stop && m_tasks.empty()) return;

            task = move(m_tasks.front());
            m_tasks.pop();
        }

        task();
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_pending--;
            if (m_pending == 0) {
                m_cv.notify_all();
            }
        }
    }
}

} // namespace mt
