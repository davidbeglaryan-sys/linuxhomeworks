#include "parallel_scheduler.h"
#include <iostream>
#include <chrono>
#include <thread>

void compute_task(int id, int sleep_ms) {
    std::cout << "Task " << id << " starting on thread [" 
              << std::this_thread::get_id() << "]" << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms)); 

    std::cout << "Task " << id << " finished on thread [" 
              << std::this_thread::get_id() << "]" << std::endl;
}

int main() {
    const size_t POOL_CAPACITY = 4;
    const size_t TOTAL_TASKS = 20;
    parallel_scheduler pool(POOL_CAPACITY);

    std::cout << "\n--- Enqueuing " << TOTAL_TASKS 
              << " tasks (Capacity is " << POOL_CAPACITY << ") ---\n" << std::endl;

    for (int i = 0; i < TOTAL_TASKS; ++i) {
        int sleep_time = 100 + (i % 5) * 10;
        pool.run(compute_task, i, sleep_time);
    }

    std::cout << "\n All tasks enqueued. Waiting for tasks to complete \n" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "\n Main thread exiting \n" << std::endl;

    return 0;
}
