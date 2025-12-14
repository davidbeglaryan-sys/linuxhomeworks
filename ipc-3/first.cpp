#include "shared_array.hpp"
#include <iostream>
#include <unistd.h>
#include <stdexcept>

constexpr size_t ARRAY_SIZE = 1000;
const std::string ARRAY_NAME = "ipc3_test_array";

int main() {
    try {
        SharedArray::unlink_resources(ARRAY_NAME); 
        SharedArray arr(ARRAY_NAME, ARRAY_SIZE);

        std::cout << "[FIRST] Writer started. PID: " << getpid() << std::endl;
        int counter = 0;

        while (true) {
            arr.lock();

            size_t index = counter % ARRAY_SIZE;
            arr[index] = counter;

            std::cout << "[FIRST] Writing: arr[" << index << "] = " << counter << std::endl;

            arr.unlock();

            counter++;
            sleep(1);
        }
    } catch (const std::exception& e) {
        std::cerr << "[FIRST] Error: " << e.what() << std::endl;
        SharedArray::unlink_resources(ARRAY_NAME);
        return 1;
    }

    return 0;
}
