#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <sys/types.h>

namespace SimpleNet {

class Socket {
public:
    Socket();
    ~Socket();

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void bind(int port);
    void listen(int backlog = 10);
    void connect(const std::string& ip, int port);

    Socket accept();

    std::vector<char> receive(size_t max_size = 4096);
    ssize_t send(std::string_view msg);

private:
    explicit Socket(int fd): fd_{fd} {}
    int fd_ = -1;
};

}
