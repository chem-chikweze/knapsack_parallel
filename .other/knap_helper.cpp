#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

using namespace std;
std::mutex mtx;

// Function to compute a range of rows in the dynamic programming table
void compute_rows(std::vector<std::vector<double>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int c)
{
    for (int i = start; i <= end; i++)
    {
        mtx.lock();
        for (int j = 1; j <= c; j++)
        {
            if (weights[i - 1] <= j)
            {
                // mtx.lock();
                dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
                // mtx.unlock();
            }
            else
            {
                // mtx.lock();
                dp[i][j] = dp[i - 1][j];
                // mtx.unlock();
            }
        }
        mtx.unlock();
    }
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <num_threads> " << endl;
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
        cerr << "Error: Could not open file " << filename << endl;
        return 1;
    }

    int n; // number of items in the knapsack
    int c; // capacity of the sack
    inputFile >> n >> c;

    std::vector<int> weights;
    std::vector<int> values;

    string line;
    while (getline(inputFile, line))
    {
        if (line.empty())
        {
            continue; // Skip empty line
        }
        stringstream ss(line);
        int weight, value;
        ss >> weight >> value;
        weights.push_back(weight);
        values.push_back(value);
    }

    inputFile.close();

    std::vector<std::vector<double>> dp(n + 1, std::vector<double>(c + 1, 0.0));

    std::vector<std::thread> threads;

    int chunk_size = n / num_threads;
    int start = 1, end = 0;

    // Create and start threads
    for (int i = 0; i < num_threads; i++)
    {
        start = end + 1;
        end = (i == num_threads - 1) ? n : start + chunk_size - 1;
        threads.emplace_back(compute_rows, std::ref(dp), std::ref(weights), std::ref(values), start, end, c);
    }

    // Join threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Output the final result
    std::cout << "Maximum value: " << dp[n][c] << std::endl;

    auto end_time = high_resolution_clock::now();
    duration<double, milli> time = end_time - start_time;

    cout << "Duration: " << time.count() << " miliseconds." << endl;

    return 0;
}


#include <thread>

// ... other includes

void compute_rows(std::vector<std::vector<double>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int c, int num_threads)
{
    // Create a thread pool with num_threads
    std::threadpool pool(num_threads);

    for (int i = start; i <= end; i++)
    {
        // Use a vector to store tasks for each iteration of the inner loop
        std::vector<std::future<void>> tasks;

        // Loop through j and schedule tasks in the thread pool
        for (int j = 1; j <= c; j++)
        {
            tasks.push_back(pool.enqueue([&, i, j, &dp, &weights, &values]() {
                if (weights[i - 1] <= j)
                {
                    dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
                }
                else
                {
                    dp[i][j] = dp[i - 1][j];
                }
            }));
        }

        // Wait for all tasks to finish before continuing to the next iteration of the outer loop
        for (auto& task : tasks)
        {
            task.get();
        }
    }
}



void compute_rows(std::vector<std::vector<double>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int c)
{
    for (int i = start; i <= end; i++)
    {
        for (int j = 1; j <= c; j++)
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

In the above function, the inner loop is dependent on the previous iteration of itself. That is i == 2 will be using values computed from i == 1
for example, dp[i][j] = max(dp[i-1][j],  values[i-1] + dp[i-1][j-weights[i-1]])
but within this inner loop, for (int j=1; j <= c; j++), there is no dependency between each iteration of the inner loop
i.e j = 2 will not depend on j = 1

I want to create parallelized version of the loop such that the inner loop will finish before another itearation of i is done.
So for i == 1, the threads will concurrently execute the inner loop. However, the threads have complete the inner loop before i == 2 can be executed.

However