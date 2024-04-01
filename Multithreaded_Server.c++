#include <iostream>
#include <string>
#include <functional>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void handleClient(int clientSocket) {
    try {
        std::string message = "Hello from server ";
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        if (getpeername(clientSocket, (struct sockaddr*)&clientAddress, &clientAddressLength) == -1) {
            throw std::runtime_error("Failed to get client address");
        }
        message += inet_ntoa(clientAddress.sin_addr);
        
        std::string response = message + "\n";
        if (send(clientSocket, response.c_str(), response.length(), 0) == -1) {
            throw std::runtime_error("Failed to send response to client");
        }
        
        close(clientSocket);
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

int main() {
    int port = 8010;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create server socket" << std::endl;
        return 1;
    }
    
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind server socket" << std::endl;
        close(serverSocket);
        return 1;
    }
    
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        close(serverSocket);
        return 1;
    }
    
    std::cout << "Server is listening on port " << port << std::endl;
    
    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }
        
        std::thread thread(handleClient, clientSocket);
        thread.detach();
    }
    
    close(serverSocket);
    
    return 0;
}

