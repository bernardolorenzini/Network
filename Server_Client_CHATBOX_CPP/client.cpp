#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>

void receiveMessages(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Server disconnected." << std::endl;
            break;
        }
        std::cout << "Server: " << buffer << std::endl;
    }
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("IP_ADDRESS_OF_SERVER"); // Replace with server's IP address

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

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

    return 0;
}
