#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int port = 8010;
    int serverSocket, acceptedSocket;
    struct sockaddr_in serverAddress, clientAddress;
    char buffer[1024] = {0};
    std::string message = "Hello World from the server";

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept connections
    socklen_t clientAddressLength = sizeof(clientAddress);
    if ((acceptedSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Print connection details
    std::cout << "Connected to " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    // Send message to client
    send(acceptedSocket, message.c_str(), message.length(), 0);

    // Close sockets
    close(acceptedSocket);
    close(serverSocket);

    return 0;
}

