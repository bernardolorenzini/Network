#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>

void receiveMessages(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Client disconnected." << std::endl;
            break;
        }
        std::cout << "Client: " << buffer << std::endl;
    }
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);

    std::cout << "Waiting for incoming connections..." << std::endl;

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    std::cout << "Client connected." << std::endl;

    std::thread receiveThread(receiveMessages, clientSocket);

    char message[1024];
    while (true) {
        memset(message, 0, sizeof(message));
        std::cout << "You: ";
        std::cin.getline(message, sizeof(message));
        send(clientSocket, message, sizeof(message), 0);
    }

    receiveThread.join();
    close(clientSocket);
    close(serverSocket);

    return 0;
}
