#include "NetworkUtils.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
#endif

void NetworkUtils::initializeNetwork() {
    try {
#ifdef _WIN32
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            throw NetworkException("WSAStartup failed: " + std::to_string(result));
        }
#endif
    } catch (const std::exception& e) {
        throw NetworkException("Error initializing network: " + std::string(e.what()));
    }
}

void NetworkUtils::cleanupNetwork() {
    try {
#ifdef _WIN32
        WSACleanup();
#endif
    } catch (const std::exception& e) {
        std::cerr << "Error cleaning up network: " << e.what() << std::endl;
    }
}

SOCKET NetworkUtils::createSocket() {
    try {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            throw NetworkException("Failed to create socket: " + getErrorMessage());
        }
        return sock;
    } catch (const std::exception& e) {
        throw NetworkException("Error creating socket: " + std::string(e.what()));
    }
}

void NetworkUtils::bindSocket(SOCKET sock, int port) {
    try {
        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        
        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            throw NetworkException("Failed to bind socket: " + getErrorMessage());
        }
    } catch (const std::exception& e) {
        throw NetworkException("Error binding socket: " + std::string(e.what()));
    }
}

void NetworkUtils::listenSocket(SOCKET sock, int backlog) {
    try {
        if (listen(sock, backlog) == SOCKET_ERROR) {
            throw NetworkException("Failed to listen on socket: " + getErrorMessage());
        }
    } catch (const std::exception& e) {
        throw NetworkException("Error listening on socket: " + std::string(e.what()));
    }
}

SOCKET NetworkUtils::acceptConnection(SOCKET serverSocket, std::string& clientIP) {
    try {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            throw NetworkException("Failed to accept connection: " + getErrorMessage());
        }
        
        clientIP = inet_ntoa(clientAddr.sin_addr);
        return clientSocket;
    } catch (const std::exception& e) {
        throw NetworkException("Error accepting connection: " + std::string(e.what()));
    }
}

SOCKET NetworkUtils::connectToServer(const std::string& serverIP, int port) {
    try {
        SOCKET sock = createSocket();
        
        struct sockaddr_in addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, serverIP.c_str(), &addr.sin_addr) <= 0) {
            closesocket(sock);
            throw NetworkException("Invalid server IP address: " + serverIP);
        }
        
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(sock);
            throw NetworkException("Failed to connect to server: " + getErrorMessage());
        }
        
        return sock;
    } catch (const std::exception& e) {
        throw NetworkException("Error connecting to server: " + std::string(e.what()));
    }
}

int NetworkUtils::sendData(SOCKET sock, const char* data, int length) {
    try {
        int bytesSent = send(sock, data, length, 0);
        if (bytesSent == SOCKET_ERROR) {
            throw NetworkException("Failed to send data: " + getErrorMessage());
        }
        return bytesSent;
    } catch (const std::exception& e) {
        throw NetworkException("Error sending data: " + std::string(e.what()));
    }
}

int NetworkUtils::receiveData(SOCKET sock, char* buffer, int length) {
    try {
        int bytesReceived = recv(sock, buffer, length, 0);
        if (bytesReceived == SOCKET_ERROR) {
            throw NetworkException("Failed to receive data: " + getErrorMessage());
        }
        return bytesReceived;
    } catch (const std::exception& e) {
        throw NetworkException("Error receiving data: " + std::string(e.what()));
    }
}

void NetworkUtils::closeSocket(SOCKET sock) {
    try {
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error closing socket: " << e.what() << std::endl;
    }
}

std::string NetworkUtils::getErrorMessage() {
#ifdef _WIN32
    int error = WSAGetLastError();
    return "Error code: " + std::to_string(error);
#else
    return std::string(strerror(errno));
#endif
}

bool NetworkUtils::isConnected(SOCKET sock) {
    if (sock == INVALID_SOCKET) {
        return false;
    }
    
    char dummy;
    int result = recv(sock, &dummy, 1, MSG_PEEK);
    return result != 0 && result != SOCKET_ERROR;
}
