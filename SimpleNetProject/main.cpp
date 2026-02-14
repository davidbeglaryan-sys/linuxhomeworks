#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "TcpServer.hpp"

int main() {
    try {
        int port = 8080;
        SimpleNet::TcpServer server(port);

        std::cout << "[SERVER] Started on port " << port << "..." << std::endl;
        std::cout << "[SERVER] Waiting for clients (Press Ctrl+C to stop)" << std::endl;

        server.run([](SimpleNet::Socket client) {
            try {
                auto data = client.receive();
                std::string message(data.begin(), data.end());
                
                std::cout << "[THREAD " << std::this_thread::get_id() 
                          << "] Received: " << message << std::endl;


                std::this_thread::sleep_for(std::chrono::seconds(2));

                std::string response = "Hello from Thread Pool! Your message was: " + message;
                client.send(response);
                
                std::cout << "[THREAD " << std::this_thread::get_id() 
                          << "] Response sent, closing connection." << std::endl;

            } catch (const std::exception& e) {
                std::cerr << "Error handling client: " << e.what() << std::endl;
            }
        });

    } catch (const std::exception& e) {
        std::cerr << "Critical server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
