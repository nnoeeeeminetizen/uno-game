#include "Client.h"
#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>

int main() {
    try {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "UNO GAME - CLIENT MODE" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::string playerName;
        std::string serverIP = "127.0.0.1";
        int serverPort = DEFAULT_PORT;
        
        // Input giocatore
        std::cout << "\nEnter your player name: ";
        std::getline(std::cin, playerName);
        
        if (playerName.empty()) {
            playerName = "Player";
        }
        
        std::cout << "\nConnecting to server at " << serverIP << ":" << serverPort << "..." << std::endl;
        
        Client client(serverIP, serverPort);
        
        if (!client.connect(playerName)) {
            std::cerr << "\nFailed to connect to server!" << std::endl;
            std::cerr << "Make sure the server is running on " << serverIP << ":" << serverPort << std::endl;
            return 1;
        }
        
        std::cout << "\nConnected successfully!" << std::endl;
        std::cout << "Joining game..." << std::endl;
        
        if (!client.joinGame()) {
            std::cerr << "\nFailed to join game!" << std::endl;
            return 1;
        }
        
        std::cout << "\nWaiting for game to start..." << std::endl;
        
        // Game loop
        while (client.isConnectedToServer()) {
            if (client.hasMessages()) {
                Message msg = client.getNextMessage();
                
                switch (msg.type) {
                    case MessageType::GAME_START:
                        std::cout << "\n*** GAME STARTED! ***" << std::endl;
                        break;
                    case MessageType::PLAYER_TURN:
                        std::cout << "\n>>> Your turn! Top card: " << msg.payload << std::endl;
                        std::cout << "Enter card index to play (or -1 to draw): ";
                        int cardIndex;
                        std::cin >> cardIndex;
                        
                        if (cardIndex == -1) {
                            client.drawCard();
                        } else {
                            client.playCard(cardIndex);
                        }
                        break;
                    case MessageType::CARD_PLAYED:
                        std::cout << "Player " << msg.playerName << " played: " << msg.payload << std::endl;
                        break;
                    case MessageType::GAME_END:
                        std::cout << "\n*** GAME OVER! ***" << std::endl;
                        std::cout << "Winner: " << msg.payload << std::endl;
                        client.disconnect();
                        return 0;
                    case MessageType::ERROR_MSG:
                        std::cerr << "Server error: " << msg.payload << std::endl;
                        break;
                    default:
                        break;
                }
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "\nDisconnected from server." << std::endl;
        
    } catch (const NetworkException& e) {
        std::cerr << "\nNetwork Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
