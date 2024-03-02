def calculate_average_time(filename):

    data = {}
    current_filename = None
    current_thread_num = None

    with open(filename, "r") as f:
        lines = f.readlines()

    for line in lines:
        stripped_line = line.strip()

        if stripped_line.endswith("ms."):
            time = float(stripped_line.split(" ")[-2])

            if current_filename and current_thread_num:
                data[current_filename][current_thread_num].append(time)

        elif stripped_line:
            file_name, thread_times = stripped_line.split(" ")
            if file_name not in data:
                data[file_name] = {}
            if thread_times not in data[file_name]:
                data[file_name][thread_times] = []

            current_filename = file_name
            current_thread_num = thread_times

    # Calculate average times for each thread in each file
    for file_name, thread_data in data.items():
        for thread_num, times in thread_data.items():
            avg_time = sum(times) / len(times)
            thread_data[thread_num] = avg_time

    return data


average_times_per_file = calculate_average_time("output_sequential.txt")

for file_name, thread_times in average_times_per_file.items():
    print(f"File: {file_name}")
    for thread_num, avg_time in thread_times.items():
        print(f"\tThread {thread_num}: {avg_time:.2f} ms")
