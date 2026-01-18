#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include "aes.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")

unsigned char AES_KEY[32] = {
    0x10,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
    0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x11,
    0x21,0x32,0x43,0x54,0x65,0x76,0x87,0x98,
    0xa9,0xba,0xcb,0xdc,0xed,0xfe,0x0f,0xf0
};

unsigned char AES_IV[16] = {
    0xa1,0xb2,0xc3,0xd4,0xe5,0xf6,0x11,0x22,
    0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa
};

AESCrypto aes(AES_KEY, AES_IV);

struct Client {
    SOCKET sock;
    std::string nick;
    std::string ip;
};

std::vector<Client> clients;
std::mutex clients_mutex;


void broadcast(const std::string& message, SOCKET sender = 0) {
    std::lock_guard<std::mutex> lock(clients_mutex);

    std::string encrypted = aes.encrypt(message);

    for (auto& c : clients) {
        if (c.sock != sender) {
            send(c.sock, encrypted.c_str(), encrypted.size(), 0);
        }
    }
}

void handleClient(Client client) {
    char buffer[2048];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int received = recv(client.sock, buffer, sizeof(buffer), 0);
        if (received <= 0) break;

        std::string decrypted = aes.decrypt(std::string(buffer, received));

        std::string fullMsg = "[" + client.nick + " " + client.ip + "]: " + decrypted;

        std::cout << fullMsg << std::endl;
        broadcast(fullMsg, client.sock);
    }

    closesocket(client.sock);

    std::string leaveMsg = "Server: [" + client.nick + " " + client.ip + "] disconnected.";
    broadcast(leaveMsg);

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove_if(clients.begin(), clients.end(), [&](Client& c) { return c.sock == client.sock; }), clients.end());
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5000);

    bind(serverSock, (sockaddr*)&server, sizeof(server));
    listen(serverSock, 5);

    std::cout << "Encrypted server running on port 5000...\n";

    while (true) {
        SOCKET clientSock = accept(serverSock, nullptr, nullptr);

        sockaddr_in addr;
        int len = sizeof(addr);
        getpeername(clientSock, (sockaddr*)&addr, &len);

        std::string ip = inet_ntoa(addr.sin_addr);

        // send prompt, encrypted
        std::string askNick = "Enter nick:";
        std::string askNickEnc = aes.encrypt(askNick);
        send(clientSock, askNickEnc.c_str(), askNickEnc.size(), 0);

        char nickBuf[256] = { 0 };
        int n = recv(clientSock, nickBuf, sizeof(nickBuf), 0);
        std::string nick = aes.decrypt(std::string(nickBuf, n));
        if (nick.empty()) nick = "Client";

        Client c{ clientSock, nick, ip };

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(c);
        }

        std::string joinMsg = "Server: [" + c.nick + " " + c.ip + "] joined!";
        broadcast(joinMsg);

        std::thread(handleClient, c).detach();
    }

    closesocket(serverSock);
    WSACleanup();
}
