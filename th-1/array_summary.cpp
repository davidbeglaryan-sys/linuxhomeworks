#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

struct ThreadData {
    int* array;         
    int start;         
    int end;           
    long long partial_sum; 
};

void* sum_array_part(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->partial_sum = 0;
    
    for (int i = data->start; i < data->end; ++i) {
        data->partial_sum += data->array[i];
    }
    
    return nullptr;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <N> <M>" << std::endl;
        return 1;
    }
    
    int N = std::atoi(argv[1]);  //str --> int
    int M = std::atoi(argv[2]);  //str --> int
    
    if (N <= 1000000) {
        std::cerr << "Error: N must be > 1000000" << std::endl;
        return 1;
    }
    
    if (M <= 0) {
        std::cerr << "Error: M must be > 0" << std::endl;
        return 1;
    }
    
    int* array = new int[N];
    std::srand(std::time(nullptr));
    for (int i = 0; i < N; ++i) {
        array[i] = std::rand() % 100; // nums 0-99
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    long long total_sum_single = 0;
    for (int i = 0; i < N; ++i) {
        total_sum_single += array[i]; // sum without threads
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    
    start_time = std::chrono::high_resolution_clock::now();
    
    pthread_t* threads = new pthread_t[M];
    ThreadData* thread_data = new ThreadData[M];
    
    int elements_per_thread = N / M;
    int remaining_elements = N % M;
    
    int current_start = 0;
    for (int i = 0; i < M; ++i) {
        thread_data[i].array = array;
        thread_data[i].start = current_start;
        
        
        // sum with threads
        if (i == M - 1) {
            thread_data[i].end = current_start + elements_per_thread + remaining_elements;
        } else {
            thread_data[i].end = current_start + elements_per_thread;
        }
        
        current_start = thread_data[i].end;
        
        pthread_create(&threads[i], nullptr, sum_array_part, &thread_data[i]);
    }
    
    for (int i = 0; i < M; ++i) {
        pthread_join(threads[i], nullptr);
    }
    
    
    long long total_sum_multi = 0;
    for (int i = 0; i < M; ++i) {
        total_sum_multi += thread_data[i].partial_sum;   //sum all threads
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    std::cout << "Time spent without threads: " << duration_single.count() << " ms" << std::endl;
    std::cout << "Time spent with " << M << " threads: " << duration_multi.count() << " ms" << std::endl;
    

    delete[] array;      //delete memory
    delete[] threads;
    delete[] thread_data;
    
    return 0;
}
