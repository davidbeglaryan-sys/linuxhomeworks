#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>   // fork, execvp, getpid, setenv
#include <sys/wait.h>  // waitpid
#include <sys/stat.h>  // open flags
#include <fcntl.h>   // open, dup2

#define MAX_ARGS 64  // max count arguments

int last_command_status = 0;


void free_args(char* args[], int count) {
    for (int i = 0; i < count; ++i) free(args[i]);
}


void execute_single_command(const std::string& command_str) {
    if (command_str.empty()) {
        last_command_status = 0;
        return;
    }
    
    std::string cmd = command_str;
    std::string output_file;
    int mode = 0; 

    size_t pos_append = cmd.find(">>");
    size_t pos_trunc = cmd.find(">");
    size_t pos_delim = std::string::npos;

    if (pos_append != std::string::npos) {
        pos_delim = pos_append;
        mode = 2; // >>
    } else if (pos_trunc != std::string::npos) {
        pos_delim = pos_trunc;
        mode = 1; // >
    }

    if (mode != 0) {
        output_file = cmd.substr(pos_delim + (mode == 2 ? 2 : 1));
        cmd = cmd.substr(0, pos_delim);
        
        size_t start = output_file.find_first_not_of(" \t");
        if (start != std::string::npos) output_file = output_file.substr(start);
        else output_file.clear();
    }

    char temp_cmd[1024];
    strncpy(temp_cmd, cmd.c_str(), sizeof(temp_cmd) - 1);
    temp_cmd[sizeof(temp_cmd) - 1] = '\0';

    char* argv[MAX_ARGS];
    int count = 0;
    
    char* token = strtok(temp_cmd, " \t");
    while (token != NULL && count < MAX_ARGS - 1) {
        argv[count++] = strdup(token); 
        token = strtok(NULL, " \t");
    }
    argv[count] = NULL; 

    if (count == 0) {
        last_command_status = 0;
        return;
    }

    bool is_silent = (strcmp(argv[0], "silent") == 0);
    char** cmd_args = argv;

    if (is_silent) {
        if (count < 2) {
            std::cerr << "shell: 'silent' needs a command." << std::endl;
            last_command_status = 1; 
            free_args(argv, count);
            return;
        }
        cmd_args = &argv[1]; 
    }

    pid_t pid = fork();

    if (pid == 0) { // child
        const char* current_path = getenv("PATH");
        std::string new_path = "./";
        if (current_path) new_path += (std::string(":") + current_path);
        setenv("PATH", new_path.c_str(), 1); 

        int fd_out = -1;
        
        if (is_silent) {
            std::string log_filename = std::to_string(getpid()) + ".log"; 
            fd_out = open(log_filename.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
        } else if (mode != 0) {
            int flags = O_CREAT | O_WRONLY;
            flags |= (mode == 2) ? O_APPEND : O_TRUNC;
            fd_out = open(output_file.c_str(), flags, 0644);
        }

        if (fd_out >= 0) {
            dup2(fd_out, STDOUT_FILENO); // STDOUT
            if (is_silent) dup2(fd_out, STDERR_FILENO); // STDERR for silent
            close(fd_out); 
        } else if (mode != 0 || is_silent) {
             perror("shell: open file failed");
             exit(EXIT_FAILURE);
        }

        // Выполнение
        execvp(cmd_args[0], cmd_args);

        perror("shell: command not found");
        exit(127); // error
    } 
    
    else { // pprocess
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_command_status = WEXITSTATUS(status);
        } else {
            last_command_status = 1;
        }
    }

    free_args(argv, count);
}

int main() {
    std::string line;
    std::cout << "Shell pid is " << getpid() <<  std::endl;
    
    while (true) {
        if (!std::getline(std::cin, line) || line.empty()) continue;

        if (line == "exit") break;

        std::vector<std::string> parts;
        size_t current_pos = 0;
        size_t op_pos;

        while (current_pos < line.length()) {
            op_pos = line.length();
            std::string op = ";"; 
            
            size_t pos_and = line.find("&&", current_pos);
            size_t pos_or = line.find("||", current_pos);
            size_t pos_semicolon = line.find(";", current_pos);

            if (pos_and != std::string::npos && pos_and < op_pos) { op_pos = pos_and; op = "&&"; }
            if (pos_or != std::string::npos && pos_or < op_pos) { op_pos = pos_or; op = "||"; }
            if (pos_semicolon != std::string::npos && pos_semicolon < op_pos) { op_pos = pos_semicolon; op = ";"; }

            std::string command = line.substr(current_pos, op_pos - current_pos);
            if (!command.empty() && command.find_first_not_of(" \t") != std::string::npos) {
                parts.push_back(command);
            }
            current_pos = op_pos + op.length();

            if (op_pos < line.length()) {
                parts.push_back(op);
            }
        }

        if (parts.empty()) continue;

        for (size_t i = 0; i < parts.size(); ++i) {
            const std::string& current_part = parts[i];

            if (current_part == "&&") {
                if (last_command_status != 0) break; 
                continue;
            }
            if (current_part == "||") {
                if (last_command_status == 0) break; 
                continue;
            }
            if (current_part == ";") {
                continue;
            }

            execute_single_command(current_part);
        }
    }
    return EXIT_SUCCESS;
}
