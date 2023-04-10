#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
/*

Client description:
  A simple implementation of a TCP client that reads command line options for server IP/port
and periodically sends a message (a sent message count) to the server every second, printing
out the server's response.

Author:
Ryan Yu
2023/04/10

How to run the code:

1. compile the code
g++ -std=c++11 client.cpp -o client

2. Run the executable
./client <server_ip> <server_port>

*/

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int server_port = std::stoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }

    int message_count = 0;
    char buffer[1024];
    while (true) {
        std::string message = "Message count: " + std::to_string(message_count++);
        send(client_socket, message.c_str(), message.size(), 0);

        memset(buffer, 0, 1024);
        int bytes_received = recv(client_socket, buffer, 1024, 0);
        if (bytes_received <= 0) {
            std::cerr << "Connection closed by server" << std::endl;
            break;
        }

        std::string server_response(buffer, bytes_received);
        std::cout << "Server response: " << server_response << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(client_socket);
    return 0;
}
