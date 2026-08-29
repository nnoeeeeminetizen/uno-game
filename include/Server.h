#ifndef SERVER_H
#define SERVER_H

#include "Game.h"
#include "NetworkUtils.h"
#include "Protocol.h"
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <memory>

class Server {
private:
    SOCKET serverSocket;
    int port;
    bool isRunning;
    std::vector<SOCKET> clientSockets;
    std::vector<std::thread> clientThreads;
    std::mutex socketMutex;
    std::mutex gameMutex;
    
    Game* activeGame;
    std::map<int, std::string> playerNames;  // playerID -> name
    std::map<SOCKET, int> socketToPlayerID;  // socket -> playerID
    int nextPlayerId;
    
    // Handler per i client
    void handleClient(SOCKET clientSocket, int playerId);
    void handleJoinGame(int playerId, const std::string& playerName, SOCKET clientSocket);
    void handlePlayCard(int playerId, const std::string& cardData);
    void handleDrawCard(int playerId);
    void handleChooseColor(int playerId, const std::string& colorData);
    void handleQuitGame(int playerId);
    
    // Broadcast ai client
    void broadcastGameState();
    void broadcastMessage(const Message& msg);
    void sendMessageToPlayer(int playerId, const Message& msg);
    
public:
    Server(int port = DEFAULT_PORT);
    ~Server();
    
    // Operazioni principali
    void start();
    void stop();
    void run();
    bool acceptClients();
    
    // Getters
    bool isServerRunning() const { return isRunning; }
    int getClientCount() const { return clientSockets.size(); }
};

#endif // SERVER_H
