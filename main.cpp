#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <future>

#include "ThreadPool.cpp"

using namespace std;

std::atomic<int> completed_tasks(0);
void compute_rows(ThreadPool &pool, std::vector<std::vector<int>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int capacity)
{
    for (int i = start; i <= end; i++)
    {
        // TODO: Too slow: 2160120
        for (int j = 1; j <= capacity; j++)
        {
            pool.enqueue([i, j, &dp, &weights, &values]()
                         {
                if (weights[i - 1] <= j)
                {
                    dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
                }
                else
                {
                    dp[i][j] = dp[i - 1][j];
                }
                completed_tasks.fetch_add(1, std::memory_order_relaxed); });
        }

        while (completed_tasks.load(std::memory_order_relaxed) != capacity)
        {
            // Spin until all tasks complete
        }
        completed_tasks.store(0); // Reset the counter for the next row
        // // pool.barrier();
        // cout << "Completed row " << i << endl;
    }
}

void compute_rows_futures(std::vector<std::vector<int>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int capacity)
{
    for (int i = start; i <= end; i++)
    {
        // // TODO: Too slow: 2160120
        // for (int j = 1; j <= capacity; j++)
        // {
        //     pool.enqueue([i, j, &dp, &weights, &values]()
        //                  {
        //         if (weights[i - 1] <= j)
        //         {
        //             dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
        //         }
        //         else
        //         {
        //             dp[i][j] = dp[i - 1][j];
        //         }
        //         completed_tasks.fetch_add(1, std::memory_order_relaxed); });
        // }

        // while (completed_tasks.load(std::memory_order_relaxed) != capacity)
        // {
        //     // Spin until all tasks complete
        // }
        // completed_tasks.store(0); // Reset the counter for the next row
        // // // pool.barrier();
        // // cout << "Completed row " << i << endl;

        std::vector<std::future<void>> futures;
        for (int w = 1; w <= capacity; ++w)
        {
            futures.push_back(std::async(
                std::launch::async, [&](int i, int w)
                {
                if (weights[i - 1] <= w)
                {
                    dp[i][w] = std::max(dp[i - 1][w], dp[i - 1][w - weights[i - 1]] + values[i - 1]);
                }
                else
                {
                    dp[i][w] = dp[i - 1][w];
                } },
                i, w));
        }
        for (auto &future : futures)
        {
            future.wait();
        }
    }
}

int knapsack(int n, int capacity, const std::vector<int> &weights, const std::vector<int> &values, int num_threads)
{
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

    for (int w = 1; w <= capacity; ++w)
    {
        std::vector<std::thread> threads;
        threads.emplace_back([&](int start_weight, int end_weight)
                             {
            for (int i = 1; i <= n; ++i)
            {
                for (int k = start_weight; k <= end_weight && k <= w; ++k)
                {
                    if (weights[i - 1] <= k)
                    {
                        dp[i][w] = std::max(dp[i - 1][w], dp[i - 1][w - weights[i - 1]] + values[i - 1]);
                    }
                    else
                    {
                        dp[i][w] = dp[i - 1][w];
                    }
                }
            } },
                             1, w);
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    return dp[n][capacity];
}

void compute_rows_sequential(std::vector<std::vector<int>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int capacity)
{
    for (int i = start; i <= end; i++)
    {
        for (int j = 1; j <= capacity; j++)
        {
            if (weights[i - 1] <= j)
            {
                dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
            }
            else
            {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <num_threads> "
             << "\n";
        return 1;
    }

    using chrono::duration;
    using chrono::high_resolution_clock;
    auto start_time = high_resolution_clock::now();

    string filename = argv[1];
    int num_threads = atoi(argv[2]);
    ifstream inputFile(filename);

    if (!inputFile.is_open())
    {
        cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }

    int n; // number of items in the knapsack
    int c; // capacity of the sack
    inputFile >> n >> c;

    std::vector<int> weights;
    std::vector<int> values;
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(c + 1, 0.0));

    string line;
    while (getline(inputFile, line))
    {
        if (line.empty())
        {
            continue;
        }
        stringstream ss(line);
        int weight, value;
        ss >> weight >> value;
        weights.push_back(weight);
        values.push_back(value);
    }

    // ThreadPool pool(num_threads, num_threads);
    // compute_rows(pool, dp, weights, values, 1, n, c);
    // compute_rows_futures(dp, weights, values, 1, n, c);
    // compute_rows_sequential(dp, weights, values, 1, n, c);
    int res = knapsack(n, c, weights, values, num_threads);

    duration<double, milli> time = high_resolution_clock::now() - start_time;
    // cout << "M: " << dp[n][c] << "\t";
    cout << "M: " << res << "\t";

    cout << "D: " << time.count() << " ms." << endl;
    return 0;
}
