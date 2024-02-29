#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

std::vector<int> weights;
std::vector<int> values;
std::vector<std::vector<int>> opt;

int knapsack(int n, int curr, int c)
{
	/*find the optimal value of the knapsack with
	items curr to n, capacity c, and weights and values given
	*/

	// base case, we are on the last item
	if (curr == n - 1)
	{
		if (weights[curr] <= c)
		{
			// take if we can
			return values[curr];
		}
		else
		{
			// leave otherwise
			return 0;
		}
	}

	if (opt[curr][c] != -1)
	{
		// value already computed
		return opt[curr][c];
	}
	// need to compute the value

	if (curr >= n || c < 0)
	{
		// Invalid state, return 0
		return 0;
	}

	// optimal value not taking item curr
	int leave = knapsack(n, curr + 1, c);
	int take = -1;

	if (c >= weights[curr])
	{
		// take the item, reducing capacity, but gaining value
		take = knapsack(n, curr + 1, c - weights[curr]) + values[curr];
	}

	if (leave > take)
	{
		opt[curr][c] = leave;
	}
	else
	{
		opt[curr][c] = take;
	}
	return opt[curr][c];
}

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " <input_file> "
			 << "\n";
		return 1;
	}

	using chrono::duration;
	using chrono::high_resolution_clock;
	auto start_time = high_resolution_clock::now();

	string filename = argv[1];
	ifstream inputFile(filename);

	if (!inputFile.is_open())
	{
		cerr << "Error: Could not open file " << filename << "\n";
		return 1;
	}

	int n; // number of items in the knapsack
	int c; // capacity of the sack
	inputFile >> n >> c;

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

	// initialize the array for dynamic programming
	opt.resize(n, std::vector<int>(c + 1, -1));

	int highest = knapsack(n, 0, c);

	auto end = high_resolution_clock::now();
	duration<double, milli> time = end - start_time;

	cout << "The maximum value is " << highest << "." << endl;
	cout << "Duration: " << time.count() << " miliseconds." << endl;
}