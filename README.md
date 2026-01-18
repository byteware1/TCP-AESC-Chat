<h1 align="center">🔐 TCP AES Chat</h1>
<p align="center">Secure encrypted TCP communication using AES‑256‑CBC and WinSock2</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows-blue?style=for-the-badge">
  <img src="https://img.shields.io/badge/Language-C++17-orange?style=for-the-badge">
  <img src="https://img.shields.io/badge/Network-TCP%2FIP-green?style=for-the-badge">
  <img src="https://img.shields.io/badge/Encryption-AES--256%20CBC-purple?style=for-the-badge">
  <img src="https://img.shields.io/badge/Build-Visual%20Studio%202022-5C2D91?style=for-the-badge">
  <img src="https://img.shields.io/badge/Library-OpenSSL%2FCrypto%2B%2B-yellow?style=for-the-badge">
</p>

<p align="center">
  <img src="https://img.shields.io/github/license/Bartek/TCPAES?style=flat-square">
  <img src="https://img.shields.io/badge/Status-Active-success?style=flat-square">
  <img src="https://img.shields.io/badge/Maintained-Yes-brightgreen?style=flat-square">
</p>

---

## 📖 Overview

TCP AES Chat is a Windows-based secure chat application written in C++17.
It supports both **server** and **client** modes with real-time encrypted messaging using **AES-256 in CBC mode**.

This project leverages:

* **WinSock2** for TCP/IP networking
* **OpenSSL / Crypto++** for AES encryption
* **Multithreading** to handle multiple clients simultaneously

It is intended for educational purposes and small-scale encrypted communication testing.

---

## 🛠 Features

* Fully functioning TCP client/server chat
* AES-256-CBC encryption for secure message transmission
* Automatic IV and key management (configurable)
* Support for multiple simultaneous clients
* Simple console-based interface
* Cross-compatibility with Windows 7, 10, and 11

---

## 💻 System Requirements

* Windows 7 / 10 / 11
* Visual Studio 2019 / 2022
* OpenSSL 1.1.x or 3.0.x
* C++17 or higher
* WinSock2 library

---

## ⚙️ Installation & Build

1. **Clone the repository**

```bash
git clone https://github.com/Bartek/TCPAES.git
cd TCPAES
```

2. **Install OpenSSL**

Download the precompiled OpenSSL binaries for Windows (1.1.1 or 3.x). Add the `include` path to Visual Studio and link `libssl.lib` and `libcrypto.lib`.

3. **Build the AES static library**

* Open the `aes-static` project
* Build → Output: `AES.lib`

4. **Link AES.lib in Server and Client**

* Go to Project → Properties → Linker → Input → Additional Dependencies
* Add `AES.lib`
* Include path: folder containing `aes.h`

5. **Build Server and Client Projects**

* Ensure all runtime libraries match (MT/MD)
* Output: `server-aes.exe` and `client-aes.exe`

---

## 🧩 Usage

### Server

```bash
server-aes.exe
```

* Listens on port 5000
* Accepts multiple clients
* Broadcasts encrypted messages to all connected clients

### Client

```bash
client-aes.exe
```

* Connects to server by IP
* Sends and receives encrypted messages
* Enter nickname at prompt (default `Client` if empty)

---

## 🔒 Encryption Details

* **Algorithm**: AES-256
* **Mode**: CBC (Cipher Block Chaining)
* **Key Size**: 256 bits
* **IV**: 128 bits, randomly generated
* **Library**: OpenSSL (via AES static wrapper)

Messages are encrypted before sending and decrypted upon receiving. AES operations are encapsulated in `AESCrypto` class.

---

## 📂 Project Structure

```
TCPAES/
├─ aes-static/          # AES library (static)
│  ├─ aes.cpp
│  └─ aes.h
├─ server-aes/          # TCP Server project
│  └─ server-aes.cpp
├─ client-aes/          # TCP Client project
│  └─ client-aes.cpp
├─ README.md
└─ .gitignore
```

---

## ⚠️ Warnings & Tips

* Make sure all projects use the **same runtime library** (MD/MT)
* Link OpenSSL libs correctly (`libssl.lib` and `libcrypto.lib`)
* For compilation warnings like `C4267`, you can safely `static_cast<int>`
* Avoid runtime conflicts by ensuring `AES.lib` is built with the same runtime
---

### Author

me
