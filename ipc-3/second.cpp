#include "shared_array.hpp"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <cstdlib>

constexpr size_t ARRAY_SIZE = 1000;
const std::string ARRAY_NAME = "ipc3_test_array";

int main() {
    try {
        SharedArray arr(ARRAY_NAME, ARRAY_SIZE); 

        std::cout << "[SECOND] Reader/Modifier started. PID: " << getpid() << std::endl;

        while (true) {
            arr.lock();

            size_t index = rand() % ARRAY_SIZE;
            int value = arr[index];

            std::cout << "[SECOND] Reading: arr[" << index << "] = " << value;

            arr[index] = value + 1;

            std::cout << " -> New value: " << arr[index] << std::endl;

            arr.unlock();

            sleep(2);
        }
    } catch (const std::exception& e) {
        std::cerr << "[SECOND] Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
