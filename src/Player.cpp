#include "Player.h"
#include <iostream>
#include <algorithm>
#include <cstring>

Player::Player(const std::string& name, int id, bool isAI)
    : name(name), playerID(id), isAI(isAI), gamesWon(0), gamesLost(0), 
      handSize(0), handCapacity(20) {
    try {
        hand = new Card*[handCapacity];
        if (!hand) {
            throw PlayerException("Failed to allocate memory for player hand");
        }
    } catch (const std::bad_alloc& e) {
        throw PlayerException("Memory allocation error in Player constructor");
    }
}

Player::~Player() {
    try {
        // Nota: Non deallochiamo le carte perché sono gestite dal Deck
        // Solo deallochiamo l'array
        if (hand != nullptr) {
            delete[] hand;
            hand = nullptr;
        }
        handSize = 0;
    } catch (const std::exception& e) {
        std::cerr << "Error during Player destruction: " << e.what() << std::endl;
    }
}

void Player::resizeHand() {
    try {
        if (handSize >= handCapacity) {
            int newCapacity = handCapacity * 2;
            Card** newHand = new Card*[newCapacity];
            if (!newHand) {
                throw PlayerException("Failed to allocate memory for resized hand");
            }
            
            for (int i = 0; i < handSize; i++) {
                newHand[i] = hand[i];
            }
            
            delete[] hand;
            hand = newHand;
            handCapacity = newCapacity;
        }
    } catch (const std::bad_alloc& e) {
        throw PlayerException("Memory allocation error during hand resize");
    }
}

void Player::addCard(Card* card) {
    try {
        if (card == nullptr) {
            throw PlayerException("Cannot add null card to player hand");
        }
        
        resizeHand();
        hand[handSize++] = card;
    } catch (const std::exception& e) {
        throw PlayerException("Error adding card: " + std::string(e.what()));
    }
}

Card* Player::playCard(int cardIndex) {
    try {
        if (cardIndex < 0 || cardIndex >= handSize) {
            throw PlayerException("Invalid card index: " + std::to_string(cardIndex));
        }
        
        Card* card = hand[cardIndex];
        
        // Rimuovi la carta dalla mano
        for (int i = cardIndex; i < handSize - 1; i++) {
            hand[i] = hand[i + 1];
        }
        handSize--;
        
        return card;
    } catch (const std::exception& e) {
        throw PlayerException("Error playing card: " + std::string(e.what()));
    }
}

void Player::removeCard(int cardIndex) {
    try {
        if (cardIndex >= 0 && cardIndex < handSize) {
            for (int i = cardIndex; i < handSize - 1; i++) {
                hand[i] = hand[i + 1];
            }
            handSize--;
        }
    } catch (const std::exception& e) {
        throw PlayerException("Error removing card: " + std::string(e.what()));
    }
}

bool Player::hasCards() const {
    return handSize > 0;
}

int Player::getHandSize() const {
    return handSize;
}

std::string Player::getName() const {
    return name;
}

int Player::getPlayerID() const {
    return playerID;
}

bool Player::getIsAI() const {
    return isAI;
}

Card** Player::getHand() const {
    return hand;
}

int Player::getGamesWon() const {
    return gamesWon;
}

int Player::getGamesLost() const {
    return gamesLost;
}

void Player::incrementWins() {
    gamesWon++;
}

void Player::incrementLosses() {
    gamesLost++;
}

void Player::resetStats() {
    gamesWon = 0;
    gamesLost = 0;
}

bool Player::canPlayCard(const Card& topCard) {
    try {
        for (int i = 0; i < handSize; i++) {
            if (hand[i] != nullptr && hand[i]->canPlayOn(topCard)) {
                return true;
            }
        }
        return false;
    } catch (const std::exception& e) {
        throw PlayerException("Error checking if player can play card: " + std::string(e.what()));
    }
}

int* Player::getValidCardIndices(const Card& topCard, int& count) {
    try {
        int* validIndices = new int[handSize];
        if (!validIndices) {
            throw PlayerException("Failed to allocate memory for valid indices");
        }
        
        count = 0;
        for (int i = 0; i < handSize; i++) {
            if (hand[i] != nullptr && hand[i]->canPlayOn(topCard)) {
                validIndices[count++] = i;
            }
        }
        
        return validIndices;
    } catch (const std::bad_alloc& e) {
        throw PlayerException("Memory allocation error in getValidCardIndices");
    }
}

int Player::chooseCard(const Card& topCard) {
    // Default implementation - può essere overridden da AI
    return -1;
}

CardColor Player::chooseWildColor() const {
    // Default implementation
    return CardColor::RED;
}

void Player::printHand() const {
    std::cout << name << "'s hand (" << handSize << " cards):" << std::endl;
    for (int i = 0; i < handSize; i++) {
        if (hand[i] != nullptr) {
            std::cout << "  [" << i << "] " << hand[i]->toString() << std::endl;
        }
    }
}

void Player::printStats() const {
    std::cout << name << " - Wins: " << gamesWon << ", Losses: " << gamesLost << std::endl;
}
