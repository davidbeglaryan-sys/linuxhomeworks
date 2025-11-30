#ifndef PARALLEL_SCHEDULER_H
#define PARALLEL_SCHEDULER_H

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>


using Task = std::function<void()>;

class parallel_scheduler {
public:

    parallel_scheduler(size_t capacity);


    ~parallel_scheduler();

    template<class F, class... Args>
    void run(F&& f, Args&&... args);

private:

    std::vector<std::thread> workers;


    std::queue<Task> tasks;


    std::mutex queue_mutex;


    std::condition_variable condition;


    bool stop_flag;

    void worker_loop();
};


template<class F, class... Args>
void parallel_scheduler::run(F&& f, Args&&... args) {

    Task task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if (stop_flag) {
            std::cerr << "Warning: Adding task to a stopped pool." << std::endl;
            return;
        }

        tasks.push(std::move(task));
    }
    condition.notify_one();
}

#endif
