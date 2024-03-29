#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

using namespace std;

int knapsack_parallel_1dim(int n, int capacity, const std::vector<int> &weights, const std::vector<int> &values, int num_threads)
{
    std::vector<int> dp(capacity + 1, 0);

    std::vector<std::thread> threads;
    for (int i = 1; i <= n; ++i)
    {
        int cols_per_thread = std::ceil(static_cast<double>(capacity) / num_threads);
        std::vector<int> current_row(capacity + 1, 0);

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
                        current_row[w] = std::max(dp[w], dp[w - weights[row - 1]] + values[row - 1]);
                    }
                    else {
                        current_row[w] = dp[w];
                    }
                } },
                                 i, start_col, end_col);
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
        threads.clear();
        dp = std::move(current_row);
    }

    return dp[capacity];
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

    int res = knapsack_parallel_1dim(n, c, weights, values, num_threads);

    duration<double, milli>
        time = high_resolution_clock::now() - start_time;
    cout << "M: " << res << "\t";
    cout << "D: " << time.count() << " ms." << endl;
    return 0;
}
