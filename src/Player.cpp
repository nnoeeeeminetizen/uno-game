#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(const std::string& name, int id, bool isAI)
    : name(name), playerID(id), isAI(isAI), gamesWon(0), gamesLost(0) {}

void Player::addCard(std::shared_ptr<Card> card) {
    hand.push_back(card);
}

std::shared_ptr<Card> Player::playCard(int cardIndex) {
    if (cardIndex < 0 || cardIndex >= static_cast<int>(hand.size())) {
        return nullptr;
    }
    
    auto card = hand[cardIndex];
    hand.erase(hand.begin() + cardIndex);
    return card;
}

void Player::removeCard(int cardIndex) {
    if (cardIndex >= 0 && cardIndex < static_cast<int>(hand.size())) {
        hand.erase(hand.begin() + cardIndex);
    }
}

bool Player::hasCards() const {
    return !hand.empty();
}

int Player::getHandSize() const {
    return hand.size();
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

std::vector<std::shared_ptr<Card>> Player::getHand() const {
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
    for (const auto& card : hand) {
        if (card->canPlayOn(topCard)) {
            return true;
        }
    }
    return false;
}

std::vector<int> Player::getValidCardIndices(const Card& topCard) {
    std::vector<int> validIndices;
    
    for (int i = 0; i < static_cast<int>(hand.size()); i++) {
        if (hand[i]->canPlayOn(topCard)) {
            validIndices.push_back(i);
        }
    }
    
    return validIndices;
}

void Player::printHand() const {
    std::cout << name << "'s hand (" << hand.size() << " cards):" << std::endl;
    for (int i = 0; i < static_cast<int>(hand.size()); i++) {
        std::cout << "  [" << i << "] " << hand[i]->toString() << std::endl;
    }
}

void Player::printStats() const {
    std::cout << name << " - Wins: " << gamesWon << ", Losses: " << gamesLost << std::endl;
}
