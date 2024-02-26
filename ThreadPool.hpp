#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
public:
    ThreadPool(size_t num_threads);

    template <class F>
    void enqueue(F &&f);

    void wait();

    ~ThreadPool();

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

#endif /* THREADPOOL_H */
