#include <iostream>
#include <cstring>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>

class Server {
private:
    std::vector<std::thread> threadPool;

public:
    Server(int poolSize) : threadPool(poolSize) {}

    void handleClient(socket_t clientSocket) {
        char message[1024];
        sprintf(message, "Hello from server %s", inet_ntop(AF_INET, &clientSocket.address.sin_addr.s_addr, buffer, BUFFER_SIZE));
        send(clientSocket, message, strlen(message), 0);
    }

    static void main(int port, int poolSize) {
        Server server(poolSize);

        socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_port = htons(port);

        bind(serverSocket, &service, sizeof(service));
        listen(serverSocket, 5);

        std::cout << "Server is listening on port " << port << std::endl;

        while (true) {
            socket_t clientSocket = accept(serverSocket, nullptr, nullptr);
            if (!clientSocket.socket) {
                break;
            }

            // Use the thread pool to handle the client
            std::thread handleThread([&clientSocket, &server = Server::getInstance()] () {
                server.handleClient(clientSocket);
                close(clientSocket.socket);
            });
            server.threadPool.push_back(std::move(handleThread));
        }

        close(serverSocket.socket);
        }
    }
};
