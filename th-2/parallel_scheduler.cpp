#include "parallel_scheduler.h"

parallel_scheduler::parallel_scheduler(size_t capacity) : stop_flag(false) {
    if (capacity == 0) {
        throw std::invalid_argument("Capacity must be greater than zero.");
    }
    
    for (size_t i = 0; i < capacity; ++i) {
        workers.emplace_back(&parallel_scheduler::worker_loop, this);
    }
    std::cout << "Scheduler initialized with " << capacity << " worker threads." << std::endl;
}

parallel_scheduler::~parallel_scheduler() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop_flag = true;
    } 

    condition.notify_all();

    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    std::cout << "Scheduler successfully stopped and threads joined." << std::endl;
}

void parallel_scheduler::worker_loop() {
    Task task;

    while (true) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            condition.wait(lock, [this]{
                return stop_flag || !tasks.empty();
            });

            if (stop_flag && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        } 

        task();
    }
}
