#pragma once
#include "Socket.hpp"
#include "ThreadPool.hpp"
#include <functional>
#include <memory> 

namespace SimpleNet {

using ClientHandler = std::function<void(Socket)>;

class TcpServer {
public:
    TcpServer(int port) : pool_(4) { 
        listen_socket_.bind(port);
        listen_socket_.listen();
    }

    void run(ClientHandler handler) {
        while (true) {
            try {
                Socket client = listen_socket_.accept();
                

                auto client_ptr = std::make_shared<Socket>(std::move(client));
                
                pool_.enqueue([handler, client_ptr]() {
                    handler(std::move(*client_ptr));
                });
            } catch (...) {
            }
        }
    }

private:
    Socket listen_socket_;
    ThreadPool pool_;
};

}
