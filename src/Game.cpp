#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <cstring>

Game::Game()
    : currentPlayerIndex(0), gameActive(false), reverseDirection(false), playersCount(0) {
    try {
        deck = new Deck();
        if (!deck) {
            throw GameException("Failed to allocate memory for Deck");
        }
        players = new Player*[4];  // Max 4 giocatori (1 umano + 3 IA)
        if (!players) {
            delete deck;
            throw GameException("Failed to allocate memory for players array");
        }
        std::memset(players, 0, sizeof(Player*) * 4);
    } catch (const std::bad_alloc& e) {
        throw GameException("Memory allocation error in Game constructor");
    }
}

Game::~Game() {
    try {
        // Dealloca i giocatori
        if (players != nullptr) {
            for (int i = 0; i < playersCount; i++) {
                if (players[i] != nullptr) {
                    delete players[i];
                    players[i] = nullptr;
                }
            }
            delete[] players;
            players = nullptr;
        }
        
        // Dealloca il mazzo
        if (deck != nullptr) {
            delete deck;
            deck = nullptr;
        }
        
        playersCount = 0;
    } catch (const std::exception& e) {
        std::cerr << "Error during Game destruction: " << e.what() << std::endl;
    }
}

void Game::initializeGame(int numAIPlayers) {
    try {
        if (numAIPlayers < 0 || numAIPlayers > 3) {
            throw GameException("Invalid number of AI players. Must be 0-3.");
        }
        
        // Crea il giocatore umano
        Player* humanPlayer = new Player("You", 0, false);
        if (!humanPlayer) {
            throw GameException("Failed to allocate memory for human player");
        }
        players[playersCount++] = humanPlayer;
        
        // Crea i giocatori IA
        const char* aiNames[] = {"AI-1", "AI-2", "AI-3"};
        AIStrategy strategies[] = {
            AIStrategy::RANDOM,
            AIStrategy::AGGRESSIVE,
            AIStrategy::SMART
        };
        
        for (int i = 0; i < numAIPlayers; i++) {
            AI* aiPlayer = new AI(aiNames[i], i + 1, strategies[i]);
            if (!aiPlayer) {
                throw GameException("Failed to allocate memory for AI player " + std::to_string(i));
            }
            players[playersCount++] = aiPlayer;
        }
        
        // Distribuisci le carte iniziali
        distributeInitialCards();
        
        // Pesca la prima carta dal discard pile
        Card* topCard = deck->drawCard();
        if (topCard) {
            deck->discardCard(topCard);
        }
        
        gameActive = true;
        currentPlayerIndex = 0;
        
        std::cout << "\nGame initialized with " << playersCount << " players!" << std::endl;
    } catch (const GameException& e) {
        throw;
    } catch (const std::exception& e) {
        throw GameException("Error initializing game: " + std::string(e.what()));
    }
}

void Game::distributeInitialCards() {
    try {
        // Distribuisci 7 carte a ogni giocatore
        for (int p = 0; p < playersCount; p++) {
            if (players[p] == nullptr) {
                throw GameException("Null player at index " + std::to_string(p));
            }
            
            for (int i = 0; i < 7; i++) {
                Card* card = deck->drawCard();
                if (card == nullptr) {
                    throw GameException("Failed to draw card for player " + std::to_string(p));
                }
                players[p]->addCard(card);
            }
        }
    } catch (const std::exception& e) {
        throw GameException("Error distributing initial cards: " + std::string(e.what()));
    }
}

Player* Game::getCurrentPlayer() const {
    if (currentPlayerIndex < 0 || currentPlayerIndex >= playersCount || players == nullptr) {
        return nullptr;
    }
    return players[currentPlayerIndex];
}

Player* Game::getNextPlayer() const {
    int nextIndex = getNextPlayerIndex();
    if (nextIndex < 0 || nextIndex >= playersCount || players == nullptr) {
        return nullptr;
    }
    return players[nextIndex];
}

int Game::getNextPlayerIndex() const {
    int nextIndex = currentPlayerIndex;
    
    if (reverseDirection) {
        nextIndex--;
        if (nextIndex < 0) {
            nextIndex = playersCount - 1;
        }
    } else {
        nextIndex++;
        if (nextIndex >= playersCount) {
            nextIndex = 0;
        }
    }
    
    return nextIndex;
}

void Game::nextTurn() {
    currentPlayerIndex = getNextPlayerIndex();
}

void Game::previousTurn() {
    currentPlayerIndex--;
    if (currentPlayerIndex < 0) {
        currentPlayerIndex = playersCount - 1;
    }
}

void Game::skipTurn() {
    nextTurn();
}

bool Game::humanPlayerTurn() {
    try {
        Player* player = getCurrentPlayer();
        if (!player || player->getIsAI()) {
            return false;
        }
        
        Card* topCard = deck->getTopCard();
        if (!topCard) {
            throw GameException("No top card available!");
        }
        
        displayGameState();
        
        std::cout << "\n" << std::string(50, '-') << std::endl;
        std::cout << "Your turn, " << player->getName() << "!" << std::endl;
        std::cout << "Top card: " << topCard->toString() << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        
        player->printHand();
        
        int validCount = 0;
        int* validIndices = player->getValidCardIndices(*topCard, validCount);
        
        if (validCount == 0) {
            std::cout << "\nNo valid cards. Drawing a card..." << std::endl;
            Card* drawnCard = deck->drawCard();
            if (drawnCard) {
                player->addCard(drawnCard);
                std::cout << "Drew: " << drawnCard->toString() << std::endl;
            }
            
            delete[] validIndices;
            Utils::pauseExecution();
            return false;  // Turno terminato
        }
        
        std::cout << "\nEnter the index of the card to play (0-" << (player->getHandSize() - 1) << "): ";
        int cardIndex = Utils::getIntInput(0, player->getHandSize() - 1);
        
        // Controlla se la carta è valida
        bool isValid = false;
        for (int i = 0; i < validCount; i++) {
            if (validIndices[i] == cardIndex) {
                isValid = true;
                break;
            }
        }
        
        if (!isValid) {
            std::cout << "Invalid card! You must play a valid card." << std::endl;
            delete[] validIndices;
            Utils::pauseExecution();
            return humanPlayerTurn();  // Riprova
        }
        
        Card* playedCard = player->playCard(cardIndex);
        deck->discardCard(playedCard);
        
        std::cout << "\nPlayed: " << playedCard->toString() << std::endl;
        
        // Gestisci carte speciali
        if (playedCard->getType() == CardType::SKIP) {
            handleSkip();
        } else if (playedCard->getType() == CardType::REVERSE) {
            handleReverse();
        } else if (playedCard->getType() == CardType::DRAW_TWO) {
            handleDrawTwo();
        } else if (playedCard->getType() == CardType::WILD) {
            handleWild(player);
        } else if (playedCard->getType() == CardType::WILD_DRAW_FOUR) {
            handleWildDrawFour(player);
        }
        
        delete[] validIndices;
        
        if (!player->hasCards()) {
            return true;  // Giocatore ha vinto
        }
        
        Utils::pauseExecution();
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error in humanPlayerTurn: " << e.what() << std::endl;
        return false;
    }
}

void Game::aiPlayerTurn() {
    try {
        Player* player = getCurrentPlayer();
        if (!player || !player->getIsAI()) {
            return;
        }
        
        Card* topCard = deck->getTopCard();
        if (!topCard) {
            throw GameException("No top card available!");
        }
        
        std::cout << "\n" << player->getName() << "'s turn..." << std::endl;
        
        // Converti il giocatore a AI
        AI* aiPlayer = dynamic_cast<AI*>(player);
        if (!aiPlayer) {
            throw GameException("Failed to cast player to AI");
        }
        
        int cardIndex = aiPlayer->chooseCard(*topCard);
        
        if (cardIndex == -1) {
            // Nessuna carta valida, pesca
            std::cout << aiPlayer->getName() << " draws a card." << std::endl;
            Card* drawnCard = deck->drawCard();
            if (drawnCard) {
                aiPlayer->addCard(drawnCard);
            }
        } else {
            Card* playedCard = aiPlayer->playCard(cardIndex);
            deck->discardCard(playedCard);
            
            std::cout << aiPlayer->getName() << " played: " << playedCard->toString() << std::endl;
            
            // Gestisci carte speciali
            if (playedCard->getType() == CardType::SKIP) {
                handleSkip();
            } else if (playedCard->getType() == CardType::REVERSE) {
                handleReverse();
            } else if (playedCard->getType() == CardType::DRAW_TWO) {
                handleDrawTwo();
            } else if (playedCard->getType() == CardType::WILD) {
                handleWild(player);
            } else if (playedCard->getType() == CardType::WILD_DRAW_FOUR) {
                handleWildDrawFour(player);
            }
        }
        
        if (player->hasCards()) {
            std::cout << aiPlayer->getName() << " has " << aiPlayer->getHandSize() << " cards left." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in aiPlayerTurn: " << e.what() << std::endl;
    }
}

void Game::handleSkip() {
    std::cout << "SKIP! Next player's turn is skipped." << std::endl;
    nextTurn();
}

void Game::handleReverse() {
    std::cout << "REVERSE! Direction changed." << std::endl;
    reverseDirection = !reverseDirection;
}

void Game::handleDrawTwo() {
    std::cout << "DRAW_TWO! Next player draws 2 cards." << std::endl;
    nextTurn();
    Player* nextPlayer = getCurrentPlayer();
    if (nextPlayer) {
        for (int i = 0; i < 2; i++) {
            Card* card = deck->drawCard();
            if (card) {
                nextPlayer->addCard(card);
            }
        }
    }
}

void Game::handleWild(Player* player) {
    try {
        std::cout << "WILD card! Player chooses a color." << std::endl;
        
        if (player->getIsAI()) {
            AI* aiPlayer = dynamic_cast<AI*>(player);
            if (aiPlayer) {
                CardColor chosenColor = aiPlayer->chooseWildColor();
                std::cout << aiPlayer->getName() << " chose: " << Utils::colorToString(chosenColor) << std::endl;
            }
        } else {
            std::cout << "Choose a color (0=RED, 1=YELLOW, 2=GREEN, 3=BLUE): ";
            int colorChoice = Utils::getIntInput(0, 3);
            CardColor chosenColor = static_cast<CardColor>(colorChoice);
            std::cout << "You chose: " << Utils::colorToString(chosenColor) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in handleWild: " << e.what() << std::endl;
    }
}

void Game::handleWildDrawFour(Player* player) {
    try {
        std::cout << "WILD_DRAW_FOUR! Player chooses a color and next player draws 4 cards." << std::endl;
        
        if (player->getIsAI()) {
            AI* aiPlayer = dynamic_cast<AI*>(player);
            if (aiPlayer) {
                CardColor chosenColor = aiPlayer->chooseWildColor();
                std::cout << aiPlayer->getName() << " chose: " << Utils::colorToString(chosenColor) << std::endl;
            }
        } else {
            std::cout << "Choose a color (0=RED, 1=YELLOW, 2=GREEN, 3=BLUE): ";
            int colorChoice = Utils::getIntInput(0, 3);
            CardColor chosenColor = static_cast<CardColor>(colorChoice);
            std::cout << "You chose: " << Utils::colorToString(chosenColor) << std::endl;
        }
        
        nextTurn();
        Player* nextPlayer = getCurrentPlayer();
        if (nextPlayer) {
            for (int i = 0; i < 4; i++) {
                Card* card = deck->drawCard();
                if (card) {
                    nextPlayer->addCard(card);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in handleWildDrawFour: " << e.what() << std::endl;
    }
}

void Game::displayGameState() const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "GAME STATE" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    for (int i = 0; i < playersCount; i++) {
        if (players[i] != nullptr) {
            std::cout << players[i]->getName() << ": " << players[i]->getHandSize() << " cards";
            if (i == currentPlayerIndex) {
                std::cout << " (CURRENT TURN)";
            }
            std::cout << std::endl;
        }
    }
    
    deck->printDeckInfo();
    std::cout << std::string(50, '=') << std::endl;
}

void Game::playRound() {
    // Placeholder - la logica principale è in playGame()
}

void Game::playGame() {
    try {
        while (gameActive) {
            Player* currentPlayer = getCurrentPlayer();
            
            if (!currentPlayer) {
                throw GameException("No current player!");
            }
            
            bool playerWon = false;
            
            if (currentPlayer->getIsAI()) {
                aiPlayerTurn();
            } else {
                playerWon = humanPlayerTurn();
            }
            
            if (playerWon) {
                std::cout << "\n" << std::string(50, '=') << std::endl;
                std::cout << currentPlayer->getName() << " WINS!" << std::endl;
                std::cout << std::string(50, '=') << std::endl;
                currentPlayer->incrementWins();
                gameActive = false;
                break;
            }
            
            // Controlla se il giocatore ha una sola carta
            if (currentPlayer->getHandSize() == 1) {
                std::cout << "\n*** " << currentPlayer->getName() << " says UNO! ***" << std::endl;
            }
            
            nextTurn();
        }
        
        // Aggiorna le statistiche per i perdenti
        Player* winner = getWinner();
        if (winner) {
            for (int i = 0; i < playersCount; i++) {
                if (players[i] != nullptr && players[i] != winner) {
                    players[i]->incrementLosses();
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during game: " << e.what() << std::endl;
        gameActive = false;
    }
}

bool Game::isGameActive() const {
    return gameActive;
}

Player* Game::getWinner() const {
    for (int i = 0; i < playersCount; i++) {
        if (players[i] != nullptr && !players[i]->hasCards()) {
            return players[i];
        }
    }
    return nullptr;
}

bool Game::saveGame(const std::string& filename) {
    // Placeholder - implementazione futura
    std::cout << "Save game not yet implemented." << std::endl;
    return false;
}

bool Game::loadGame(const std::string& filename) {
    // Placeholder - implementazione futura
    std::cout << "Load game not yet implemented." << std::endl;
    return false;
}

void Game::printGameState() const {
    displayGameState();
}

void Game::printPlayerStats() const {
    std::cout << "\nPlayer Statistics:" << std::endl;
    for (int i = 0; i < playersCount; i++) {
        if (players[i] != nullptr) {
            players[i]->printStats();
        }
    }
}

void Game::resetScores() {
    try {
        for (int i = 0; i < playersCount; i++) {
            if (players[i] != nullptr) {
                players[i]->resetStats();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error resetting scores: " << e.what() << std::endl;
    }
}
