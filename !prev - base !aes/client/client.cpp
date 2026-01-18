#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

bool running = true;

void receiveThread(SOCKET sock) {
    char buffer[1024];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            std::cout << "\nServer has shut down!\n";
            running = false;
            break;
        }
        std::cout << "\r" << buffer << "\n>> ";
        std::cout.flush();
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Cannot create socket!\n";
        return 1;
    }

    std::string ip;
    std::cout << "Enter server IP: ";
    std::cin >> ip;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5000);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Cannot connect to server!\n";
        return 1;
    }

    std::cin.ignore();

    // receive nickname prompt
    char buffer[256] = { 0 };
    recv(sock, buffer, sizeof(buffer), 0);
    std::cout << buffer;

    std::string nick;
    std::getline(std::cin, nick);
    if (nick.empty()) nick = "Client";
    send(sock, nick.c_str(), nick.size(), 0);

    std::thread recvThread(receiveThread, sock);

    while (running) {
        std::cout << ">> ";
        std::string msg;
        std::getline(std::cin, msg);
        if (!running) break;
        send(sock, msg.c_str(), msg.size(), 0);
    }

    closesocket(sock);
    recvThread.join();
    WSACleanup();
    return 0;
}
