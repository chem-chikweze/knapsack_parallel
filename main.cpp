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

// TODO: CODE CLEANUP
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

int knapsack_parallel(int n, int capacity, const std::vector<int> &weights, const std::vector<int> &values, int num_threads)
{
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

    std::vector<std::thread> threads;
    for (int i = 1; i <= n; ++i)
    {
        int cols_per_thread = std::ceil(static_cast<double>(capacity) / num_threads);
        for (int t = 0; t < num_threads; ++t)
        {
            int start_col = t * cols_per_thread + 1;
            int end_col = std::min((t + 1) * cols_per_thread, capacity);

            threads.emplace_back([&](int row, int start_col, int end_col)
                                 {
                for (int w = start_col; w <= end_col; ++w)
                {
                    if (weights[row - 1] <= w)
                    {
                        dp[row][w] = std::max(dp[row - 1][w], dp[row - 1][w - weights[row - 1]] + values[row - 1]);
                    }
                    else
                    {
                        dp[row][w] = dp[row - 1][w];
                    }
                } },
                                 i, start_col, end_col);
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
        threads.clear();
    }

    return dp[n][capacity];
}

int knapsack_sequential(int n, int capacity, const std::vector<int> &weights, const std::vector<int> &values)
{
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; i++)
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

    return dp[n][capacity];
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
    // std::vector<std::vector<int>> dp(n + 1, std::vector<int>(c + 1, 0.0));

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

    // int res = knapsack_sequential(n, c, weights, values);
    int res = knapsack_parallel(n, c, weights, values, num_threads);

    duration<double, milli>
        time = high_resolution_clock::now() - start_time;
    // cout << "M: " << dp[n][c] << "\t";
    cout << "M: " << res << "\t";

    cout << "D: " << time.count() << " ms." << endl;
    return 0;
}
