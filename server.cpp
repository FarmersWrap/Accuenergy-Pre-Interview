#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*

Server description:
  A simple implementation of a TCP server that listens on port 8080,
accepts multiple clients, and echoes back clients' messages.

Author:
Ryan Yu
2023/04/10

How to run the code:

1. compile the code
g++ -std=c++11 server.cpp -o server -pthread

2. Run the executable
./server

*/

void handle_client(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, 1024);
        int bytes_read = recv(client_socket, buffer, 1024, 0);
        if (bytes_read <= 0) {
            break;
        }
        std::string message(buffer, bytes_read);
        std::cout << "Received message from client: " << message << std::endl;
        send(client_socket, buffer, bytes_read, 0);
    }
    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error binding socket" << std::endl;
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        return 1;
    }

    std::cout << "Server listening on port 8080" << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            std::cerr << "Error accepting client" << std::endl;
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Client connected from " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    close(server_socket);
    return 0;
}
