#!/bin/bash                    


# Extract argument
input_folder=inputs

# Check if input folder exists
if [ ! -d "$input_folder" ]; then
  echo "Input folder $input_folder not found"
  exit 1
fi

# Compile the knap_helper.cpp file
g++ -std=c++11 knapsack_sequential.cpp -o my_program_sequential 

# Function to run knap_helper for a given input file and thread count
run_knap_helper() {
  input_file=$1
  for i in {1}; do
    ./my_program_sequential "$input_file" >> output_sequential.txt
  done
}

# Run knap_helper for each input file with different thread counts
for input_file in "$input_folder"/*.txt; do
  filename=${input_file##*/}
  # Loop through each thread count in the list
  echo "$filename " >> output_sequential.txt
  echo "Running knapsack for $input_file with 1 threads"
  run_knap_helper "$input_file"
  echo ""
done
