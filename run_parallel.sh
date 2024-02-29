#!/bin/bash

#SBATCH --job-name=knap_benchmark_parallel  
#SBATCH --output=parallel_knap_output.log     
#SBATCH --error=parallel_knap_error.log     
#SBATCH --time=02:00:00             
#SBATCH --cpus-per-task=1          
#SBATCH --mem=8G                   

# List of thread counts to iterate through
thread_counts=(1 2 4 8 16 )

# Extract argument
input_folder="inputs"

# Check if input folder exists
if [ ! -d "$input_folder" ]; then
  echo "Input folder $input_folder not found"
  exit 1
fi

# Compile the knap_helper.cpp file
g++ -std=c++11 main.cpp -o my_program -pthread

# Function to run knap_helper for a given input file and thread count
run_knap_helper() {
  input_file=$1
  num_threads=$2
  for i in {1..10}; do
    ./my_program "$input_file" "$num_threads" >> output.txt
  done
}

# Run knap_helper for each input file with different thread counts
for input_file in "$input_folder"/*.txt; do
  filename=${input_file##*/}

  # Loop through each thread count in the list
  for num_threads in "${thread_counts[@]}"; do
    echo "$filename $num_threads" >> output.txt
    echo "Running knapsack for $input_file with $num_threads threads"
    run_knap_helper "$input_file" "$num_threads"
    echo ""
  done
done
