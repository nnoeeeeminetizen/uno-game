#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <memory>

Game::Game()
    : currentPlayerIndex(0), gameActive(false), reverseDirection(false) {
    deck = std::make_shared<Deck>();
}

Game::~Game() {}

void Game::initializeGame(int numAIPlayers) {
    // Crea il giocatore umano
    players.push_back(std::make_shared<Player>("You", 0, false));
    
    // Crea i giocatori IA
    std::vector<std::string> aiNames = {"AI-1", "AI-2", "AI-3"};
    std::vector<AIStrategy> strategies = {
        AIStrategy::RANDOM,
        AIStrategy::AGGRESSIVE,
        AIStrategy::SMART
    };
    
    for (int i = 0; i < numAIPlayers && i < 3; i++) {
        players.push_back(std::make_shared<AI>(aiNames[i], i + 1, strategies[i]));
    }
    
    // Distribuisci le carte iniziali
    distributeInitialCards();
    
    // Mescola il mazzo
    deck->resetDeck();
    
    // Pesca la prima carta dal discard pile
    auto topCard = deck->drawCard();
    deck->discardCard(topCard);
    
    gameActive = true;
    currentPlayerIndex = 0;
}

void Game::distributeInitialCards() {
    // Distribuisci 7 carte a ogni giocatore
    for (auto& player : players) {
        for (int i = 0; i < 7; i++) {
            auto card = deck->drawCard();
            if (card) {
                player->addCard(card);
            }
        }
    }
}

Player* Game::getCurrentPlayer() const {
    if (currentPlayerIndex < 0 || currentPlayerIndex >= static_cast<int>(players.size())) {
        return nullptr;
    }
    return players[currentPlayerIndex].get();
}

Player* Game::getNextPlayer() const {
    int nextIndex = getNextPlayerIndex();
    if (nextIndex < 0 || nextIndex >= static_cast<int>(players.size())) {
        return nullptr;
    }
    return players[nextIndex].get();
}

int Game::getNextPlayerIndex() const {
    int nextIndex = currentPlayerIndex;
    
    if (reverseDirection) {
        nextIndex--;
        if (nextIndex < 0) {
            nextIndex = players.size() - 1;
        }
    } else {
        nextIndex++;
        if (nextIndex >= static_cast<int>(players.size())) {
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
        currentPlayerIndex = players.size() - 1;
    }
}

void Game::skipTurn() {
    nextTurn();
}

bool Game::humanPlayerTurn() {
    Player* player = getCurrentPlayer();
    if (!player || player->getIsAI()) {
        return false;
    }
    
    auto topCard = deck->getTopCard();
    if (!topCard) {
        std::cout << "ERROR: No top card!" << std::endl;
        return false;
    }
    
    displayGameState();
    
    std::cout << "\n" << std::string(50, '-') << std::endl;
    std::cout << "Your turn, " << player->getName() << "!" << std::endl;
    std::cout << "Top card: " << topCard->toString() << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    player->printHand();
    
    auto validIndices = player->getValidCardIndices(*topCard);
    
    if (validIndices.empty()) {
        std::cout << "\nNo valid cards. Drawing a card..." << std::endl;
        auto drawnCard = deck->drawCard();
        if (drawnCard) {
            player->addCard(drawnCard);
            std::cout << "Drew: " << drawnCard->toString() << std::endl;
        }
        Utils::pauseExecution();
        return false;  // Turno terminato
    }
    
    std::cout << "\nEnter the index of the card to play: ";
    int cardIndex = Utils::getIntInput(0, player->getHandSize() - 1);
    
    if (std::find(validIndices.begin(), validIndices.end(), cardIndex) == validIndices.end()) {
        std::cout << "Invalid card! You must play a valid card." << std::endl;
        Utils::pauseExecution();
        return humanPlayerTurn();  // Riprova
    }
    
    auto playedCard = player->playCard(cardIndex);
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
        handleWild(std::static_pointer_cast<Player>(players[currentPlayerIndex]));
    } else if (playedCard->getType() == CardType::WILD_DRAW_FOUR) {
        handleWildDrawFour(std::static_pointer_cast<Player>(players[currentPlayerIndex]));
    }
    
    if (!player->hasCards()) {
        return true;  // Giocatore ha vinto
    }
    
    Utils::pauseExecution();
    return false;
}

void Game::aiPlayerTurn() {
    Player* player = getCurrentPlayer();
    if (!player || !player->getIsAI()) {
        return;
    }
    
    auto topCard = deck->getTopCard();
    if (!topCard) {
        std::cout << "ERROR: No top card!" << std::endl;
        return;
    }
    
    std::cout << "\n" << player->getName() << "'s turn..." << std::endl;
    
    // Converti il giocatore a AI
    AI* aiPlayer = dynamic_cast<AI*>(player);
    if (!aiPlayer) {
        return;
    }
    
    int cardIndex = aiPlayer->chooseCard(*topCard);
    
    if (cardIndex == -1) {
        // Nessuna carta valida, pesca
        std::cout << aiPlayer->getName() << " draws a card." << std::endl;
        auto drawnCard = deck->drawCard();
        if (drawnCard) {
            aiPlayer->addCard(drawnCard);
        }
    } else {
        auto playedCard = aiPlayer->playCard(cardIndex);
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
            handleWild(players[currentPlayerIndex]);
        } else if (playedCard->getType() == CardType::WILD_DRAW_FOUR) {
            handleWildDrawFour(players[currentPlayerIndex]);
        }
    }
    
    if (player->hasCards()) {
        std::cout << aiPlayer->getName() << " has " << aiPlayer->getHandSize() << " cards left." << std::endl;
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
            auto card = deck->drawCard();
            if (card) {
                nextPlayer->addCard(card);
            }
        }
    }
}

void Game::handleWild(std::shared_ptr<Player> player) {
    std::cout << "WILD card! Player chooses a color." << std::endl;
    
    if (player->getIsAI()) {
        AI* aiPlayer = dynamic_cast<AI*>(player.get());
        CardColor chosenColor = aiPlayer->chooseWildColor();
        std::cout << aiPlayer->getName() << " chose: " << Utils::colorToString(chosenColor) << std::endl;
    } else {
        std::cout << "Choose a color (0=RED, 1=YELLOW, 2=GREEN, 3=BLUE): ";
        int colorChoice = Utils::getIntInput(0, 3);
        CardColor chosenColor = static_cast<CardColor>(colorChoice);
        std::cout << "You chose: " << Utils::colorToString(chosenColor) << std::endl;
    }
}

void Game::handleWildDrawFour(std::shared_ptr<Player> player) {
    std::cout << "WILD_DRAW_FOUR! Player chooses a color and next player draws 4 cards." << std::endl;
    
    if (player->getIsAI()) {
        AI* aiPlayer = dynamic_cast<AI*>(player.get());
        CardColor chosenColor = aiPlayer->chooseWildColor();
        std::cout << aiPlayer->getName() << " chose: " << Utils::colorToString(chosenColor) << std::endl;
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
            auto card = deck->drawCard();
            if (card) {
                nextPlayer->addCard(card);
            }
        }
    }
}

void Game::displayGameState() const {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "GAME STATE" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    for (const auto& player : players) {
        std::cout << player->getName() << ": " << player->getHandSize() << " cards";
        if (player == players[currentPlayerIndex]) {
            std::cout << " (CURRENT TURN)";
        }
        std::cout << std::endl;
    }
    
    deck->printDeckInfo();
    std::cout << std::string(50, '=') << std::endl;
}

void Game::playRound() {
    // Placeholder - la logica principale sarà in playGame()
}

void Game::playGame() {
    while (gameActive) {
        Player* currentPlayer = getCurrentPlayer();
        
        if (!currentPlayer) {
            break;
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
}

bool Game::isGameActive() const {
    return gameActive;
}

std::shared_ptr<Player> Game::getWinner() const {
    for (const auto& player : players) {
        if (!player->hasCards()) {
            return player;
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
    for (const auto& player : players) {
        player->printStats();
    }
}

void Game::resetScores() {
    for (auto& player : players) {
        player->resetStats();
    }
}
