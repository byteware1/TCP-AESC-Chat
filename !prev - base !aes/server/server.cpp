#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")

struct Client {
    SOCKET sock;
    std::string nick;
    std::string ip;
};

std::vector<Client> clients;
std::mutex clients_mutex;

void broadcast(const std::string& message, SOCKET sender = 0) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& c : clients) {
        if (c.sock != sender) {
            send(c.sock, message.c_str(), message.size(), 0);
        }
    }
}

void handleClient(Client client) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(client.sock, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        std::string msg = "[" + client.nick + " " + client.ip + "]: " + std::string(buffer);
        std::cout << msg << std::endl;
        broadcast(msg, client.sock);
    }

    closesocket(client.sock);

    std::string disconnectMsg = "Server: [" + client.nick + " " + client.ip + "] has disconnected.\n";
    std::cout << disconnectMsg;
    broadcast(disconnectMsg);

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [&](Client& c) { return c.sock == client.sock; }), clients.end());
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Cannot create socket!\n";
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(5000);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed!\n";
        return 1;
    }

    listen(serverSocket, 5);
    std::cout << "Server is running... (port 5000)\n";

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) continue;

        char ipStr[16];
        sockaddr_in addr;
        int addrSize = sizeof(addr);
        getpeername(clientSocket, (sockaddr*)&addr, &addrSize);
        strcpy(ipStr, inet_ntoa(addr.sin_addr));

        // ask client for nick
        char nickBuffer[256] = { 0 };
        send(clientSocket, "Enter your nickname (or leave empty): ", 50, 0);
        int nickLen = recv(clientSocket, nickBuffer, sizeof(nickBuffer), 0);
        std::string nick = (nickLen > 0) ? std::string(nickBuffer, nickLen) : "/NoName";
        if (nick.empty() || nick == "\n" || nick == "\r") nick = "/NoName";

        Client client{ clientSocket, nick, ipStr };

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client);
        }

        std::string connectMsg = "Server: [" + client.nick + " " + client.ip + "] connected!\n";
        std::cout << connectMsg;
        broadcast(connectMsg, client.sock);

        std::thread(handleClient, client).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
