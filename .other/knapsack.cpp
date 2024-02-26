#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

using namespace std;

int* weights;
int* values;
int** opt;

int knapsack(int n, int curr, int c)
{
	/*find the optimal value of the knapsack with
	items curr to n, capacity c, and weights and values given
	*/
	
	//base case, we are on the last item
	if(curr == n - 1)
	{
		if(weights[curr] <= c)
		{
			//take if we can
			return values[curr];
		}
		else
		{
			//leave otherwise
			return 0;
		}
	}
	
	if(opt[curr][c] != -1)
	{
		//value already computed
		return opt[curr][c];
	}
	//need to compute the value
	
	//optimal value not taking item curr
	int leave = knapsack(n, curr + 1, c);
	int take = -1;
	
	if(c >= weights[curr])
	{
		//take the item, reducing capacity, but gaining value
		take = knapsack(n, curr + 1, c-weights[curr]) + values[curr];
	}
	
	if(leave > take)
	{
		opt[curr][c] = leave;
	}
	else
	{
		opt[curr][c] = take;
	}
	return opt[curr][c];
}

int main(int argc, char* argv[])
{
	if (argc != 3) {
		cerr << "Usage: " <<argv[0] << " <input_file> <num_threads> " << endl;
		return 1;
	}
	
	using chrono::high_resolution_clock;
	using chrono::duration;
	auto start = high_resolution_clock::now();

	string filename = argv[1];
	int num_threads = atoi(argv[2]);
	ifstream inputFile(filename);

	if (!inputFile.is_open()) {
		cerr << "Error: Could not open file " << filename << endl;
		return 1;
	}

	int n ; //number of items in the knapsack
	int c ; //capacity of the sack
	inputFile >> n >> c;
	
	std::vector<int> weights;
	std::vector<int> values;

	string line;
	while (getline(inputFile, line)) {
		if (line.empty()) {
			continue; // Skip empty line
		}
		stringstream ss(line);
		int weight, value;
		ss >> weight >> value;
		weights.push_back(weight);
		values.push_back(value);
	}

	/*TEST: confirming values of n, c, num_threads, weights and values*/
	// cout << n << "  " << c << endl;
	// for (int element: weights) {
	// 	cout << element << endl;
	// }
	// for (int element: values) {
	// 	cout << element << endl;
	// }

	inputFile.close();
	return 0;
	
	//initialize the array for dynamic programming
	opt = new int*[n];
	for(int i=0; i<n; i++)
	{
		opt[i] = new int[c+1];
		//intialize to -1
		for(int j=0; j<c+1; j++)
		{
			opt[i][j] = -1;
		}
	}
	
	int highest = knapsack(n, 0, c);
	
	auto end = high_resolution_clock::now();
	duration<double, milli> time = end - start;
	
	cout << "The maximum value is " << highest << "." << endl;
	cout<<"Duration: "<<time.count() <<" miliseconds."<<endl;
}

// bhg0045 has 52
// bhg0049 has 48
// bhg0050, bhg0051, bhg0052, bhg0053 has 56
// bhg0059, bhg0060, bhg0061, bhg0063, bhx0060 has 64


