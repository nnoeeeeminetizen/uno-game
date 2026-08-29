#include <iostream>
#include "Game.h"
#include "Utils.h"

void printMainMenu() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "          WELCOME TO UNO CARD GAME" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "1. Start New Game\n";
    std::cout << "2. Load Saved Game\n";
    std::cout << "3. View Statistics\n";
    std::cout << "4. Exit\n";
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Choose an option (1-4): ";
}

int main() {
    int choice = 0;
    
    while (true) {
        printMainMenu();
        choice = Utils::getIntInput(1, 4);
        
        switch (choice) {
            case 1: {
                std::cout << "\nStarting new game...\n";
                Game game;
                game.initializeGame(3);  // 1 human + 3 AI
                game.playGame();
                break;
            }
            case 2: {
                std::cout << "\nLoad game feature coming soon!\n";
                Utils::pauseExecution();
                break;
            }
            case 3: {
                std::cout << "\nStatistics feature coming soon!\n";
                Utils::pauseExecution();
                break;
            }
            case 4: {
                std::cout << "\nThanks for playing! Goodbye!\n";
                return 0;
            }
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;
}
