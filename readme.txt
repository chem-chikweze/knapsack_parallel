
The parallel version of knapsack is in knapsack_parallel.cpp
It can be compiled using:
    g++ -std=c++11 knapsack_parallel.cpp -o my_program -pthread
It can then be run using:
        ./my_program_parallel "$input_file" "$num_threads" 
        where "input_file" is the input file 
        "num_threads" is the number of threads

To make it easier, I included the logic for compiling and running it in run_parallel.sh
Within run_parallel.sh, the parallel version will output the Max value and time spent to output_parallel.txt
You can then run using:
    ./run_parallel.sh
You might want to make it executable first using:
    chmod +x run_parallel.sh

The sequential version is similar.
Compiling it is: 
    g++ -std=c++11 knapsack_sequential.cpp -o my_program_sequential 
Running it is:
    ./my_program_sequential "$input_file" 
    where "input_file" is the input file name

I also included another shell script to make running the sequential program easier.
You can run the sequential program using the shell script:
    ./run_sequential.sh 

You might want to make it executable first using:
    chmod +x run_sequential.sh


The outputs of the shell scripts from the parallel program and the sequential program 
will be "output_parallel.txt" and "output_sequential.txt"

You can then get the file name, thread count and average time "calculate_parallel_average_time.py" python script



The code:
The code uses bottom up dynamic programming with space optimizations.
The parallelization happens for each row. 
    On each row, we have a new row to hold the current results
    Each row is broken up into chunks of columns which are assigned to threads. 
    The threads work independently on their assigned columns 

    After the res

After the threads finish their columns, the result is merged and the dp takes on the current row.
That way we save space and only use 1dimension (capacity) instead of N * capacity.

