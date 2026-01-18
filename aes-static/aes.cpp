#include "aes.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <cstring>

AESCrypto::AESCrypto(const unsigned char* k, const unsigned char* i) {
    memcpy(key, k, 32);
    memcpy(iv, i, 16);
}

std::string AESCrypto::encrypt(const std::string& plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> out(plaintext.size() + 16);
    int len1 = 0, len2 = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
    EVP_EncryptUpdate(ctx, out.data(), &len1, (unsigned char*)plaintext.data(), plaintext.size());
    EVP_EncryptFinal_ex(ctx, out.data() + len1, &len2);

    EVP_CIPHER_CTX_free(ctx);
    return std::string((char*)out.data(), len1 + len2);
}

std::string AESCrypto::decrypt(const std::string& ciphertext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> out(ciphertext.size() + 16);
    int len1 = 0, len2 = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
    EVP_DecryptUpdate(ctx, out.data(), &len1, (unsigned char*)ciphertext.data(), ciphertext.size());
    EVP_DecryptFinal_ex(ctx, out.data() + len1, &len2);

    EVP_CIPHER_CTX_free(ctx);
    return std::string((char*)out.data(), len1 + len2);
}
