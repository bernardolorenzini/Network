#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>

void receiveMessages(int clientSocket, QTextEdit *messageTextEdit) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cerr << "Client disconnected." << std::endl;
            break;
        }
        QString receivedMessage = QString::fromUtf8(buffer);
        messageTextEdit->append("Client: " + receivedMessage);
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Server GUI");

    QWidget *centralWidget = new QWidget(&window);
    window.setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QTextEdit *messageTextEdit = new QTextEdit(centralWidget);
    layout->addWidget(messageTextEdit, 0, Qt::AlignCenter);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    layout->addLayout(buttonLayout);

    QPushButton *sendButton = new QPushButton("Send", centralWidget);
    buttonLayout->addWidget(sendButton);

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

    std::thread receiveThread(receiveMessages, clientSocket, messageTextEdit);

    QObject::connect(sendButton, &QPushButton::clicked, [&]() {
        QString message = messageTextEdit->toPlainText();
        send(clientSocket, message.toUtf8().constData(), message.length(), 0);
    });

    window.show();

    int result = app.exec();

    receiveThread.join();
    close(clientSocket);
    close(serverSocket);

    return result;
}
