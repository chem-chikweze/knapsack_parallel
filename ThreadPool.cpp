#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t num_threads, size_t barrier_threshold) : stop(false)
{
    this->barrier_threshold = barrier_threshold;
    for (size_t i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(
            [this]
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this]
                                             { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                        {
                            return;
                        }
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task();
                }
            });
    }
}

template <class F>
void ThreadPool::enqueue(F &&f)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace(std::forward<F>(f));
    }
    condition.notify_one();
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : threads)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::barrier()
{
    std::unique_lock<std::mutex> lock(barrier_mutex);

    if (tasks.empty() && std::all_of(threads.begin(), threads.end(), [](std::thread &t)
                                     { return !t.joinable(); }))
    {
        barrier_condition.notify_all();
    }
    else
    {
        barrier_condition.wait(lock, [this]
                               { return tasks.empty() && std::all_of(threads.begin(), threads.end(),
                                                                     [](std::thread &t)
                                                                     { return !t.joinable(); }); });
    }
}

size_t ThreadPool::getBarrier()
{
    return this->tasks.size();
}

void ThreadPool::wait()
{
    std::unique_lock<std::mutex> lock(queue_mutex);

    if (tasks.empty() && std::all_of(threads.begin(), threads.end(), [](std::thread &t)
                                     { return !t.joinable(); }))
    {
        condition.notify_all();
    }
    else
    {
        condition.wait(lock, [this]
                       { return tasks.empty() && std::all_of(threads.begin(), threads.end(),
                                                             [](std::thread &t)
                                                             { return !t.joinable(); }); });
    }
}

template <class F>
void ThreadPool::enqueueBatch(const std::vector<F> &batch_tasks)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        for (const auto &task : batch_tasks)
        {
            tasks.emplace(task);
        }
    }
    condition.notify_all();
}

// # MOD: changed to || instead of &&
// void ThreadPool::wait()
// {
//     {
//         std::unique_lock<std::mutex> lock(queue_mutex);
//         stop = true;
//     }
//     condition.notify_all();
//     for (std::thread &worker : threads)
//     {
//         if (worker.joinable()) {
//              worker.join();
//         }
//     }
// }

// ThreadPool::~ThreadPool()
// {
//     wait();
// }
