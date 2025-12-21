#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

void* receive_messages(void* arg) {
    int sock = *((int*)arg);
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            std::cout << "\n Disconnected from server \n";
            exit(0);
        }
        std::cout << buffer << std::flush;
    }
    return nullptr;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(8080)};
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    std::cout << "Enter your name: ";
    std::string name;
    std::getline(std::cin, name);
    send(sock, name.c_str(), name.size(), 0);

    pthread_t tid;
    pthread_create(&tid, nullptr, receive_messages, &sock);

    std::string msg;
    while (std::getline(std::cin, msg)) {
        if (msg == "/exit") break;
        send(sock, msg.c_str(), msg.size(), 0);
    }

    close(sock);
    return 0;
}
