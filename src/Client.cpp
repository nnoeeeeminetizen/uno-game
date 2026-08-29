#include "Client.h"
#include <iostream>
#include <cstring>

Client::Client(const std::string& ip, int port)
    : socket(INVALID_SOCKET), serverIP(ip), serverPort(port),
      playerId(-1), isConnected(false), gameRunning(false) {
    try {
        NetworkUtils::initializeNetwork();
    } catch (const NetworkException& e) {
        throw;
    }
}

Client::~Client() {
    try {
        disconnect();
        NetworkUtils::cleanupNetwork();
    } catch (const std::exception& e) {
        std::cerr << "Error in Client destructor: " << e.what() << std::endl;
    }
}

bool Client::connect(const std::string& name) {
    try {
        playerName = name;
        socket = NetworkUtils::connectToServer(serverIP, serverPort);
        isConnected = true;
        
        std::cout << "Connected to server at " << serverIP << ":" << serverPort << std::endl;
        
        // Avvia il thread di ricezione messaggi
        receiveThread = std::thread(&Client::receiveMessages, this);
        
        return true;
    } catch (const NetworkException& e) {
        std::cerr << "Failed to connect: " << e.what() << std::endl;
        isConnected = false;
        return false;
    }
}

void Client::disconnect() {
    try {
        isConnected = false;
        gameRunning = false;
        
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
        
        if (socket != INVALID_SOCKET) {
            NetworkUtils::closeSocket(socket);
            socket = INVALID_SOCKET;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error disconnecting: " << e.what() << std::endl;
    }
}

bool Client::joinGame() {
    try {
        Message msg(MessageType::JOIN_GAME, playerName, "Joining game");
        return sendMessage(msg);
    } catch (const std::exception& e) {
        std::cerr << "Error joining game: " << e.what() << std::endl;
        return false;
    }
}

bool Client::playCard(int cardIndex) {
    try {
        std::string data = std::to_string(cardIndex);
        Message msg(MessageType::PLAY_CARD, playerName, data, playerId);
        return sendMessage(msg);
    } catch (const std::exception& e) {
        std::cerr << "Error playing card: " << e.what() << std::endl;
        return false;
    }
}

bool Client::drawCard() {
    try {
        Message msg(MessageType::DRAW_CARD, playerName, "", playerId);
        return sendMessage(msg);
    } catch (const std::exception& e) {
        std::cerr << "Error drawing card: " << e.what() << std::endl;
        return false;
    }
}

bool Client::chooseWildColor(CardColor color) {
    try {
        std::string colorStr = std::to_string(static_cast<int>(color));
        Message msg(MessageType::CHOOSE_COLOR, playerName, colorStr, playerId);
        return sendMessage(msg);
    } catch (const std::exception& e) {
        std::cerr << "Error choosing color: " << e.what() << std::endl;
        return false;
    }
}

bool Client::quitGame() {
    try {
        Message msg(MessageType::QUIT_GAME, playerName, "", playerId);
        sendMessage(msg);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error quitting game: " << e.what() << std::endl;
        return false;
    }
}

bool Client::sendMessage(const Message& msg) {
    try {
        if (!isConnected) {
            throw NetworkException("Not connected to server");
        }
        
        NetworkUtils::sendData(socket, (const char*)&msg, sizeof(Message));
        return true;
    } catch (const NetworkException& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
        isConnected = false;
        return false;
    }
}

bool Client::receiveMessage(Message& msg) {
    try {
        if (!isConnected) {
            return false;
        }
        
        char buffer[BUFFER_SIZE];
        int bytesReceived = NetworkUtils::receiveData(socket, buffer, BUFFER_SIZE);
        
        if (bytesReceived <= 0) {
            isConnected = false;
            return false;
        }
        
        std::memcpy(&msg, buffer, sizeof(Message));
        return true;
    } catch (const NetworkException& e) {
        std::cerr << "Error receiving message: " << e.what() << std::endl;
        isConnected = false;
        return false;
    }
}

void Client::receiveMessages() {
    try {
        while (isConnected) {
            Message msg;
            if (receiveMessage(msg)) {
                {
                    std::lock_guard<std::mutex> lock(messageMutex);
                    messageQueue.push(msg);
                }
                handleMessage(msg);
            } else {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in receive loop: " << e.what() << std::endl;
        isConnected = false;
    }
}

void Client::handleMessage(const Message& msg) {
    try {
        switch (msg.type) {
            case MessageType::GAME_START:
                handleGameStart(msg);
                break;
            case MessageType::PLAYER_TURN:
                handlePlayerTurn(msg);
                break;
            case MessageType::CARD_PLAYED:
                handleCardPlayed(msg);
                break;
            case MessageType::GAME_END:
                handleGameEnd(msg);
                break;
            case MessageType::ERROR_MSG:
                handleError(msg);
                break;
            default:
                std::cout << "Received message: " << (int)msg.type << std::endl;
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling message: " << e.what() << std::endl;
    }
}

void Client::handleGameStart(const Message& msg) {
    std::cout << "Game started! " << msg.payload << std::endl;
    gameRunning = true;
}

void Client::handlePlayerTurn(const Message& msg) {
    std::cout << "Your turn! Top card: " << msg.payload << std::endl;
}

void Client::handleCardPlayed(const Message& msg) {
    std::cout << "Player " << msg.playerName << " played: " << msg.payload << std::endl;
}

void Client::handleGameEnd(const Message& msg) {
    std::cout << "Game ended! Winner: " << msg.payload << std::endl;
    gameRunning = false;
}

void Client::handleError(const Message& msg) {
    std::cerr << "Server error: " << msg.payload << std::endl;
}

Message Client::getNextMessage() {
    std::lock_guard<std::mutex> lock(messageMutex);
    if (messageQueue.empty()) {
        return Message();
    }
    
    Message msg = messageQueue.front();
    messageQueue.pop();
    return msg;
}
