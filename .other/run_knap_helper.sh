#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input folder>"
    exit 1
fi

# Extract argument
input_folder=$1

# Check if input folder exists
if [ ! -d "$input_folder" ]; then
    echo "Input folder $input_folder not found"
    exit 1
fi

# Compile the knap_helper.cpp file
g++ -std=c++11 -pthread knap_helper.cpp -o knap_helper

# Function to run knap_helper for a given input file and number of threads
run_knap_helper() {
    input_file=$1
    num_threads=$2
    avg_time=0
    for i in {1..10}; do
        start=$(date +%s.%N)
        ./knap_helper "$input_file" "$num_threads"
        end=$(date +%s.%N)
        runtime=$(echo "$end - $start" | bc)
        avg_time=$(echo "$avg_time + $runtime" | bc)
    done
    avg_time=$(echo "scale=4; $avg_time / 10" | bc)
    echo "Average execution time for $input_file with $num_threads threads: $avg_time seconds"
}

# Run knap_helper for each input file with different thread counts
for input_file in "$input_folder"/*.txt; do
    echo "Input file: $input_file"
    for num_threads in 1 2 4 8 16; do
        echo "Running knap_helper with $num_threads threads"
        run_knap_helper "$input_file" "$num_threads"
    done
    echo ""
done
