#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <algorithm>

struct Client {
    int socket;
    std::string name;
};

std::vector<Client> clients;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const std::string& message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (const auto& client : clients) {
        if (client.socket != sender_fd) {
            send(client.socket, message.c_str(), message.size(), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void* handle_client(void* arg) {
    int client_fd = *((int*)arg);
    delete (int*)arg;
    char buffer[1024];
    
    int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        close(client_fd);
        return nullptr;
    }
    buffer[n] = '\0';
    std::string client_name(buffer);

    pthread_mutex_lock(&clients_mutex);
    clients.push_back({client_fd, client_name});
    pthread_mutex_unlock(&clients_mutex);

    std::string welcome = "Server" + client_name + " joined the chat.\n";
    std::cout << welcome;
    broadcast(welcome, client_fd);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;

        std::string msg(buffer);
        if (msg == "/list") {
            std::string list = "Server Active users: ";
            pthread_mutex_lock(&clients_mutex);
            for (const auto& c : clients) list += c.name + " ";
            pthread_mutex_unlock(&clients_mutex);
            list += "\n";
            send(client_fd, list.c_str(), list.size(), 0);
        } else {
            std::string formatted = "[" + client_name + "]: " + msg + "\n";
            broadcast(formatted, client_fd);
        }
    }

    pthread_mutex_lock(&clients_mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
                  [client_fd](const Client& c) { return c.socket == client_fd; }),
                  clients.end());
    pthread_mutex_unlock(&clients_mutex);

    std::string exit_msg = "Server " + client_name + " left the chat.\n";
    std::cout << exit_msg;
    broadcast(exit_msg, -1);
    close(client_fd);
    return nullptr;
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
    std::cout << "Server Chat started on port 8080..." << std::endl;

    while (true) {
        int* client_fd = new int(accept(server_fd, nullptr, nullptr));
        pthread_t tid;
        pthread_create(&tid, nullptr, handle_client, client_fd);
        pthread_detach(tid);
    }
    return 0;
}
