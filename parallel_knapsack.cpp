#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <queue>

struct Item {
  int value;
  int weight;
};

int knapsack(const std::vector<Item>& items, int capacity, int start, int end) {
  std::vector<int> table(capacity + 1, 0);

  for (int i = start; i < end; ++i) {
    for (int j = capacity; j >= items[i].weight; --j) {
      table[j] = std::max(table[j], table[j - items[i].weight] + items[i].value);
    }
  }

  return table[capacity];
}

int parallelKnapsack(const std::vector<Item>& items, int capacity) {
  int num_threads = std::thread::hardware_concurrency();
  std::vector<WorkQueue> work_queues(num_threads); // Array of WorkQueue objects

  std::vector<std::future<int>> futures;

  // Divide items into chunks (adjust granularity as needed)
  int chunk_size = std::max(1UL, items.size() / (2 * num_threads));
  for (int i = 0; i < items.size(); i += chunk_size) {
    // TODO: Ensure that the static cast is within range of test datasets
    int end = std::min(i + chunk_size, static_cast<int>(items.size()));
    work_queues[i / chunk_size].push(i, end); // Assign work to specific queue
  }

  // Launch threads
  for (int i = 0; i < num_threads; ++i) {
    futures.push_back(std::async(std::launch::async, [&work_queues, &items, capacity, i, num_threads] {
      int start, end;
      while (work_queues[i].pop(start, end)) {
        int value = knapsack(items, capacity, start, end);
        return value;
      }
      // No more work from queue, try stealing from other threads
      while (true) {
        int victim_thread = (i + 1) % num_threads;
        if (victim_thread == i) {
          break; // No point stealing from ourselves
        }
        // Attempt to steal work from the victim thread
        if (work_queues[victim_thread].pop(start, end)) {
          int value = knapsack(items, capacity, start, end);
          return value;
        }
        // Help the victim thread if it has significantly less work
        work_queues[i].help(work_queues[victim_thread]);
      }
      return 0; // No work found, return 0
    }));
  }

  // Wait for all tasks to finish and aggregate the maximum value
  int max_value = 0;
  for (auto& future : futures) {
    int value = future.get();
    max_value = std::max(max_value, value);
  }

  return max_value;
}

class WorkQueue {
public:
  void push(int start, int end) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push(std::make_pair(start, end));
  }

  bool pop(int& start, int& end) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (tasks_.empty()) {
      return false;
    }
    start = tasks_.front().first;
    end = tasks_.front().second;
    tasks_.pop();
    return true;
  }

  // Help a thread replenish its work queue (optional)
  void help(WorkQueue& other_queue) {
    std::lock_guard<std::mutex> lock1(mutex_);
    std::lock_guard<std::mutex> lock2(other_queue.mutex_);
    if (tasks_.size() > other_queue.tasks_.size() * 2) {
      // Transfer some tasks if the stealing thread has significantly less work
      int num_tasks_to_transfer = tasks_.size() / 4;
      for (int i = 0; i < num_tasks_to_transfer; ++i) {
        other_queue.tasks_.push(tasks_.front());
        tasks_.pop();
      }
    }
  }

private:
  std::queue<std::pair<int, int>> tasks_;
  std::mutex mutex_;
};

int main() {
  std::vector<Item> items = {{60, 10}, {100, 20}, {120, 30}};
  int capacity = 50;

  int max_value = parallelKnapsack(items, capacity);

  std::cout << "Maximum value that can be obtained: " << max_value << std::endl;

  return 0;
}
