#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>


double calculate_duration(const timeval& start, const timeval& end) {

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;

    return (double)seconds + (double)microseconds / 1000000.0;
}

void do_command(char** argv) {

    timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp failed");
        exit(127);
    }

    else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
        }

        gettimeofday(&end_time, NULL);

        double duration = calculate_duration(start_time, end_time);

        int exit_code = 0;
        if (WIFEXITED(status)) {

            exit_code = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            exit_code = 128 + WTERMSIG(status);
        }
        std::cout << "\nCommand completed with " << exit_code
                  << " exit code and took " << duration << " seconds." << std::endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command> [arg1] [arg2] ..." << std::endl;
        return EXIT_FAILURE;
    }

    do_command(&argv[1]);

    return EXIT_SUCCESS;
}
