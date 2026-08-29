#ifndef CLIENT_H
#define CLIENT_H

#include "NetworkUtils.h"
#include "Protocol.h"
#include "Card.h"
#include <string>
#include <thread>
#include <mutex>
#include <queue>

class Client {
private:
    SOCKET socket;
    std::string serverIP;
    int serverPort;
    int playerId;
    std::string playerName;
    bool isConnected;
    bool gameRunning;
    
    std::thread receiveThread;
    std::mutex messageMutex;
    std::queue<Message> messageQueue;
    
    // Handler messaggi
    void receiveMessages();
    void handleMessage(const Message& msg);
    void handleGameStart(const Message& msg);
    void handlePlayerTurn(const Message& msg);
    void handleCardPlayed(const Message& msg);
    void handleGameEnd(const Message& msg);
    void handleError(const Message& msg);
    
    // Utility
    bool sendMessage(const Message& msg);
    bool receiveMessage(Message& msg);
    
public:
    Client(const std::string& ip = "127.0.0.1", int port = DEFAULT_PORT);
    ~Client();
    
    // Connessione
    bool connect(const std::string& name);
    void disconnect();
    bool isConnectedToServer() const { return isConnected; }
    
    // Azioni di gioco
    bool joinGame();
    bool playCard(int cardIndex);
    bool drawCard();
    bool chooseWildColor(CardColor color);
    bool quitGame();
    
    // Getters
    std::string getPlayerName() const { return playerName; }
    int getPlayerId() const { return playerId; }
    bool isGameRunning() const { return gameRunning; }
    bool hasMessages() const { return !messageQueue.empty(); }
    Message getNextMessage();
};

#endif // CLIENT_H
