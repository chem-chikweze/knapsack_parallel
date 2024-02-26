#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include "ThreadPool.cpp"

using namespace std;

std::atomic<int> completed_tasks(0);

void compute_rows(ThreadPool &pool, std::vector<std::vector<int>> &dp, const std::vector<int> &weights, const std::vector<int> &values, int start, int end, int capacity)
{
    for (int i = start; i <= end; i++)
    {
        // cout << "starting row:  " << i << endl;

        // Loop through j and enqueue tasks for each iteration
        for (int j = 1; j <= capacity; j++)
        {
            // cout << pool.getBarrier() << "\n";
            pool.enqueue([i, j, &dp, &weights, &values]()
                         {
                if (weights[i - 1] <= j)
                {
                    // cout << "i: " << i <<" j: "<< j << " (i-1)(j): " << dp[i - 1][j]  << values[i - 1] + dp[i - 1][j - weights[i - 1]] << "\n";
                    dp[i][j] = std::max(dp[i - 1][j], values[i - 1] + dp[i - 1][j - weights[i - 1]]);
                }
                else
                {   
                    // cout << "i: " << i <<" j: " << j << "(i,j): " << dp[i][j] << "\n";
                    dp[i][j] = dp[i - 1][j];
                } 
                completed_tasks.fetch_add(1, std::memory_order_relaxed);
                });
                // cout << j << endl;
        }
        // cout << "Added all tasks for row: " << i << endl;
        
        // Wait for all tasks in this row to finish
        while (completed_tasks.load(std::memory_order_relaxed) != capacity) {
            // Spin until all tasks complete
        }
        completed_tasks.store(0); // Reset the counter for the next row
        // cout << pool.getBarrier() << "\n";
        // pool.barrier();
        // cout << "Completed row " << i << endl;
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
            continue; // Skip empty line
        }
        stringstream ss(line);
        int weight, value;
        ss >> weight >> value;
        weights.push_back(weight);
        values.push_back(value);
    }

    // cout << "Weights: " << weights.size() << ", num_threads: " << num_threads << ", n: " << n << "\n";
    ThreadPool pool(num_threads, num_threads);
    compute_rows(pool, dp, weights, values, 1, n, c);
    // pool.wait();

    duration<double, milli> time = high_resolution_clock::now() - start_time;
    cout << "M: " << dp[n][c] << "\t";
    cout<<"D: "<<time.count() <<" ms."<<endl;
    return 0;
}

// g++ -std=c++11 main.cpp ThreadPool.cpp -o my_program -pthread
// g++ -std=c++11 main.cpp -o my_program -pthread