#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <string>
#include <stdexcept>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    typedef int ssize_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    typedef int SOCKET;
#endif

// Exception per errori di rete
class NetworkException : public std::runtime_error {
public:
    explicit NetworkException(const std::string& message) : std::runtime_error(message) {}
};

namespace NetworkUtils {
    // Inizializzazione
    void initializeNetwork();
    void cleanupNetwork();
    
    // Socket operations
    SOCKET createSocket();
    void bindSocket(SOCKET sock, int port);
    void listenSocket(SOCKET sock, int backlog);
    SOCKET acceptConnection(SOCKET serverSocket, std::string& clientIP);
    SOCKET connectToServer(const std::string& serverIP, int port);
    
    // Send/Receive
    int sendData(SOCKET sock, const char* data, int length);
    int receiveData(SOCKET sock, char* buffer, int length);
    
    // Utility
    void closeSocket(SOCKET sock);
    std::string getErrorMessage();
    bool isConnected(SOCKET sock);
}

#endif // NETWORKUTILS_H
