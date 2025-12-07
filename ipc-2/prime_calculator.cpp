#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cmath>
#include <cstring>
#include <cstdlib> 

#define READ_END 0
#define WRITE_END 1

bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int calculate_mth_prime(int m) {
    if (m <= 0) return 0;

    int count = 0; 
    int num = 1;  

    while (count < m) {
        num++;
        if (is_prime(num)) {
            count++;
        }
    }
    return num;
}

void child_process(int read_fd_m, int write_fd_result) {
    std::cout << "[Child] Child process started." << std::endl;
    int m;
    ssize_t bytes_read;

    while ((bytes_read = read(read_fd_m, &m, sizeof(m))) > 0) {
        if (bytes_read == sizeof(m)) {
            std::cout << "[Child] Calculating " << m << "-th prime number..." << std::endl;

            int result_prime = calculate_mth_prime(m);
            
            std::cout << "[Child] Sending calculation result of prime(" << m << ")..." << std::endl;
            ssize_t bytes_written = write(write_fd_result, &result_prime, sizeof(result_prime));

            if (bytes_written != sizeof(result_prime)) {
                std::cerr << "[Child] ERROR: Failed to write result back to parent." << std::endl;
            }
        } else {
            std::cerr << "[Child] Warning: Incomplete read from pipe." << std::endl;
        }
    }
    
    if (bytes_read == 0) {
        std::cout << "[Child] Parent pipe closed. Exiting child process." << std::endl;
    } else if (bytes_read == -1) {
        perror("[Child] ERROR: Read from pipe failed");
    }

    exit(0); 
}

void parent_process(int write_fd_m, int read_fd_result, pid_t child_pid) {
    std::string input;
    
    std::cout << "[Parent] Prime calculator started. Child PID: " << child_pid << std::endl;

    while (true) {
        std::cout << "[Parent] Please enter the number (m) or 'exit': ";
        
        if (!std::getline(std::cin, input)) {
            break;
        }

        if (input == "exit") {
            std::cout << "[Parent] 'exit' command received. Shutting down." << std::endl;
            break;
        }
        
        int m;
        try {
            m = std::stoi(input);
            if (m <= 0) {
                 std::cout << "[Parent] Please enter a positive integer." << std::endl;
                 continue;
            }
        } catch (const std::exception& e) {
            std::cout << "[Parent] Invalid input. Please enter an integer or 'exit'." << std::endl;
            continue;
        }
        
        std::cout << "[Parent] Sending " << m << " to the child process..." << std::endl;
        ssize_t bytes_written = write(write_fd_m, &m, sizeof(m));
        
        if (bytes_written != sizeof(m)) {
             std::cerr << "[Parent] ERROR: Failed to write to pipe. Child might be dead." << std::endl;
             break;
        }

        std::cout << "[Parent] Waiting for the response from the child process..." << std::endl;
        int result_prime;
        ssize_t bytes_read = read(read_fd_result, &result_prime, sizeof(result_prime));
        
        if (bytes_read == 0) {
            std::cerr << "[Parent] ERROR: Child pipe closed unexpectedly. Child process might have crashed." << std::endl;
            break;
        } else if (bytes_read == -1) {
            perror("[Parent] ERROR: Read from pipe failed");
            break;
        }
        
        std::cout << "[Parent] Received calculation result of prime " << m << " = " 
                  << result_prime << ".\n" << std::endl;
    }
    
    close(write_fd_m);
    close(read_fd_result);

    kill(child_pid, SIGTERM); 
    
    waitpid(child_pid, NULL, 0); 
    std::cout << "[Parent] Child process terminated. Exiting." << std::endl;
}

int main() {
    int p_to_c[2]; 
    int c_to_p[2];

    pid_t pid;

    if (pipe(p_to_c) == -1 || pipe(c_to_p) == -1) {
        perror("Error creating pipes");
        return 1;
    }
    
    pid = fork();
    
    if (pid < 0) {
        perror("Error forking process");
        return 1;
    } 
    
    else if (pid > 0) {
        close(p_to_c[READ_END]); 
        close(c_to_p[WRITE_END]); 

        parent_process(p_to_c[WRITE_END], c_to_p[READ_END], pid);        
    } 
    

    else {
        close(p_to_c[WRITE_END]); 
        close(c_to_p[READ_END]); 
        child_process(p_to_c[READ_END], c_to_p[WRITE_END]);
            }
    
    return 0;
}
