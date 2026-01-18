#pragma once
#ifndef AES_HELPER_H
#define AES_HELPER_H

#include <string>
#include <vector>
#include <openssl/evp.h>

class AESCrypto {
private:
    unsigned char key[32];
    unsigned char iv[16];

public:
    AESCrypto(const unsigned char* k, const unsigned char* i);
    std::string encrypt(const std::string& plaintext);
    std::string decrypt(const std::string& ciphertext);
};

#endif
#pragma once
