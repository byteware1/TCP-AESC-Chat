#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
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

bool running = true;

void recvThread(SOCKET s) {
    char buffer[2048];

    while (running) {
        int r = recv(s, buffer, sizeof(buffer), 0);
        if (r <= 0) {
            std::cout << "\nDisconnected!\n";
            running = false;
            break;
        }

        std::string decrypted = aes.decrypt(std::string(buffer, r));
        std::cout << "\r" << decrypted << "\n>> ";
        std::cout.flush();
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);


    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    std::string ip;
    std::cout << "Enter server IP: ";
    std::cin >> ip;
    std::cin.ignore();

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    char buffer[256];
    int r = recv(sock, buffer, sizeof(buffer), 0);
    std::string ask = aes.decrypt(std::string(buffer, r));
    std::cout << ask << "\n";

    std::string nick;
    std::getline(std::cin, nick);
    std::string nickEnc = aes.encrypt(nick);
    send(sock, nickEnc.c_str(), nickEnc.size(), 0);

    std::thread th(recvThread, sock);

    while (running) {
        std::cout << ">> ";
        std::string msg;
        std::getline(std::cin, msg);

        if (!running) break;

        std::string enc = aes.encrypt(msg);
        send(sock, enc.c_str(), enc.size(), 0);
    }

    closesocket(sock);
    th.join();
    WSACleanup();
}
