#include "Server.h"
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <cstring>

Server::Server(int port)
    : serverSocket(INVALID_SOCKET), port(port), isRunning(false),
      activeGame(nullptr), nextPlayerId(0) {
    try {
        NetworkUtils::initializeNetwork();
        serverSocket = NetworkUtils::createSocket();
        NetworkUtils::bindSocket(serverSocket, port);
        NetworkUtils::listenSocket(serverSocket, BACKLOG);
        
        std::cout << "Server created on port " << port << std::endl;
    } catch (const NetworkException& e) {
        throw;
    } catch (const std::exception& e) {
        throw NetworkException("Error creating server: " + std::string(e.what()));
    }
}

Server::~Server() {
    try {
        stop();
        
        // Chiudi tutti i socket dei client
        {
            std::lock_guard<std::mutex> lock(socketMutex);
            for (SOCKET sock : clientSockets) {
                NetworkUtils::closeSocket(sock);
            }
            clientSockets.clear();
        }
        
        // Chiudi il socket del server
        if (serverSocket != INVALID_SOCKET) {
            NetworkUtils::closeSocket(serverSocket);
        }
        
        // Elimina il gioco attivo
        if (activeGame != nullptr) {
            delete activeGame;
            activeGame = nullptr;
        }
        
        NetworkUtils::cleanupNetwork();
    } catch (const std::exception& e) {
        std::cerr << "Error in Server destructor: " << e.what() << std::endl;
    }
}

void Server::start() {
    try {
        if (isRunning) {
            throw NetworkException("Server is already running");
        }
        
        isRunning = true;
        std::cout << "Server started. Waiting for connections..." << std::endl;
        
        // Avvia il loop di accettazione client
        while (isRunning) {
            if (!acceptClients()) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error starting server: " << e.what() << std::endl;
        isRunning = false;
    }
}

void Server::stop() {
    try {
        isRunning = false;
        
        // Attendi che tutti i thread terminino
        for (auto& thread : clientThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        clientThreads.clear();
    } catch (const std::exception& e) {
        std::cerr << "Error stopping server: " << e.what() << std::endl;
    }
}

bool Server::acceptClients() {
    try {
        std::string clientIP;
        SOCKET clientSocket = NetworkUtils::acceptConnection(serverSocket, clientIP);
        
        std::cout << "Client connected from " << clientIP << std::endl;
        
        int playerId = nextPlayerId++;
        
        {
            std::lock_guard<std::mutex> lock(socketMutex);
            clientSockets.push_back(clientSocket);
            socketToPlayerID[clientSocket] = playerId;
        }
        
        // Crea un thread per gestire questo client
        std::thread t(&Server::handleClient, this, clientSocket, playerId);
        clientThreads.push_back(std::move(t));
        
        return isRunning;
    } catch (const NetworkException& e) {
        std::cerr << "Error accepting client: " << e.what() << std::endl;
        return false;
    }
}

void Server::handleClient(SOCKET clientSocket, int playerId) {
    try {
        std::cout << "Handling client " << playerId << std::endl;
        
        char buffer[BUFFER_SIZE];
        while (isRunning) {
            std::memset(buffer, 0, BUFFER_SIZE);
            int bytesReceived = NetworkUtils::receiveData(clientSocket, buffer, BUFFER_SIZE - 1);
            
            if (bytesReceived <= 0) {
                std::cout << "Client " << playerId << " disconnected" << std::endl;
                break;
            }
            
            // Parse il messaggio
            Message* msg = (Message*)buffer;
            
            switch (msg->type) {
                case MessageType::JOIN_GAME:
                    handleJoinGame(playerId, msg->playerName, clientSocket);
                    break;
                case MessageType::PLAY_CARD:
                    handlePlayCard(playerId, msg->payload);
                    break;
                case MessageType::DRAW_CARD:
                    handleDrawCard(playerId);
                    break;
                case MessageType::CHOOSE_COLOR:
                    handleChooseColor(playerId, msg->payload);
                    break;
                case MessageType::QUIT_GAME:
                    handleQuitGame(playerId);
                    break;
                default:
                    std::cerr << "Unknown message type from client " << playerId << std::endl;
                    break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling client " << playerId << ": " << e.what() << std::endl;
    } finally {
        // Pulisci il client
        {
            std::lock_guard<std::mutex> lock(socketMutex);
            NetworkUtils::closeSocket(clientSocket);
            socketToPlayerID.erase(clientSocket);
        }
    }
}

void Server::handleJoinGame(int playerId, const std::string& playerName, SOCKET clientSocket) {
    try {
        std::lock_guard<std::mutex> lock(gameMutex);
        
        playerNames[playerId] = playerName;
        
        // Se questo è il primo giocatore, crea una nuova partita
        if (activeGame == nullptr) {
            activeGame = new Game();
            activeGame->initializeGame(0);  // Nessuna IA in multiplayer
        }
        
        // Notifica ai client che il gioco è iniziato
        if (clientSockets.size() >= 2) {
            Message startMsg(MessageType::GAME_START, playerName, "Game started", playerId);
            broadcastMessage(startMsg);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling join game: " << e.what() << std::endl;
    }
}

void Server::handlePlayCard(int playerId, const std::string& cardData) {
    try {
        std::lock_guard<std::mutex> lock(gameMutex);
        
        if (activeGame) {
            // Processa la mossa nel gioco
            std::cout << "Player " << playerId << " played card: " << cardData << std::endl;
            broadcastGameState();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling play card: " << e.what() << std::endl;
    }
}

void Server::handleDrawCard(int playerId) {
    try {
        std::lock_guard<std::mutex> lock(gameMutex);
        
        if (activeGame) {
            std::cout << "Player " << playerId << " draws a card" << std::endl;
            broadcastGameState();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling draw card: " << e.what() << std::endl;
    }
}

void Server::handleChooseColor(int playerId, const std::string& colorData) {
    try {
        std::lock_guard<std::mutex> lock(gameMutex);
        
        std::cout << "Player " << playerId << " chose color: " << colorData << std::endl;
        broadcastGameState();
    } catch (const std::exception& e) {
        std::cerr << "Error handling choose color: " << e.what() << std::endl;
    }
}

void Server::handleQuitGame(int playerId) {
    try {
        std::lock_guard<std::mutex> lock(gameMutex);
        
        playerNames.erase(playerId);
        std::cout << "Player " << playerId << " quit the game" << std::endl;
        
        if (playerNames.empty()) {
            if (activeGame != nullptr) {
                delete activeGame;
                activeGame = nullptr;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling quit game: " << e.what() << std::endl;
    }
}

void Server::broadcastGameState() {
    try {
        Message stateMsg(MessageType::GAME_STATE, "Server", "Game state updated");
        broadcastMessage(stateMsg);
    } catch (const std::exception& e) {
        std::cerr << "Error broadcasting game state: " << e.what() << std::endl;
    }
}

void Server::broadcastMessage(const Message& msg) {
    try {
        std::lock_guard<std::mutex> lock(socketMutex);
        
        for (SOCKET sock : clientSockets) {
            try {
                NetworkUtils::sendData(sock, (const char*)&msg, sizeof(Message));
            } catch (const NetworkException& e) {
                std::cerr << "Failed to send broadcast message: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error broadcasting message: " << e.what() << std::endl;
    }
}

void Server::sendMessageToPlayer(int playerId, const Message& msg) {
    try {
        std::lock_guard<std::mutex> lock(socketMutex);
        
        for (SOCKET sock : clientSockets) {
            if (socketToPlayerID[sock] == playerId) {
                NetworkUtils::sendData(sock, (const char*)&msg, sizeof(Message));
                return;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error sending message to player: " << e.what() << std::endl;
    }
}
