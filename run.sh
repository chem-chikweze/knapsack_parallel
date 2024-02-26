#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <input folder> <num_threads>"
    exit 1
fi

# Extract argument
input_folder=$1
num_threads=$2

# Check if input folder exists
if [ ! -d "$input_folder" ]; then
    echo "Input folder $input_folder not found"
    exit 1
fi

# Compile the knap_helper.cpp file
g++ -std=c++11 main.cpp -o my_program -pthread

# Function to run knap_helper for a given input file and number of threads
run_knap_helper() {
    input_file=$1
    num_threads=$2
    for i in {1..100}; do
        ./my_program "$input_file" "$num_threads" >> output.txt
    done
}

# Run knap_helper for each input file with different thread counts
for input_file in "$input_folder"/*.txt; do
    filename=${input_file##*/} 
    echo "$filename $num_threads" >> output.txt
  
    echo "Running knapsack for $input_file with $num_threads threads"
    run_knap_helper "$input_file" "$num_threads"
    echo ""
done






# for i in {1..10}; do
#     ./my_program inputs/2.txt 8 >> output.txt
# done

# /**Weights: 10000, num_threads: 8, n: 10000
# Maximum for dp: 304865
# Weights: 10000, num_threads: 8, n: 10000
# Maximum for dp: 304865
# Weights: 10000, num_threads: 8, n: 10000
# Maximum for dp: 304865
# Weights: 10000, num_threads: 8, n: 10000
# Maximum for dp: 304865
# Weights: 10000, num_threads: 8, n: 10000
# Maximum for dp: 304865
