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
    ThreadPool(size_t num_threads, size_t barrier_threshold);
    ~ThreadPool();

    template <class F>
    void enqueue(F &&f);

    void wait();
    void barrier();

    size_t getBarrier();

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    std::mutex barrier_mutex;
    std::condition_variable barrier_condition;
    size_t barrier_count;
    size_t barrier_threshold;
};

#endif /* THREADPOOL_H */
