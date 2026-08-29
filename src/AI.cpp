#include "AI.h"
#include "Utils.h"
#include <algorithm>
#include <map>

AI::AI(const std::string& name, int id, AIStrategy strategy)
    : Player(name, id, true), strategy(strategy) {}

int AI::selectCardRandom(const Card& topCard) {
    auto validIndices = getValidCardIndices(topCard);
    
    if (validIndices.empty()) {
        return -1;  // No valid card
    }
    
    int randomIndex = Utils::getRandomInt(0, validIndices.size() - 1);
    return validIndices[randomIndex];
}

int AI::selectCardAggressive(const Card& topCard) {
    auto validIndices = getValidCardIndices(topCard);
    
    if (validIndices.empty()) {
        return -1;
    }
    
    // Preferisci carte speciali
    for (int idx : validIndices) {
        if (getHand()[idx]->isSpecial()) {
            return idx;
        }
    }
    
    // Se non ci sono carte speciali, gioca una carta a caso
    int randomIndex = Utils::getRandomInt(0, validIndices.size() - 1);
    return validIndices[randomIndex];
}

int AI::selectCardSmart(const Card& topCard) {
    auto validIndices = getValidCardIndices(topCard);
    
    if (validIndices.empty()) {
        return -1;
    }
    
    // Priorità 1: Gioca carte che riducono la mano
    if (getHandSize() > 5) {
        // Se hai tante carte, preferisci le speciali
        for (int idx : validIndices) {
            if (getHand()[idx]->isSpecial()) {
                return idx;
            }
        }
    }
    
    // Priorità 2: Mantieni la diversità di colori
    CardColor topColor = topCard.getColor();
    std::vector<int> sameColor;
    
    for (int idx : validIndices) {
        if (getHand()[idx]->getColor() == topColor) {
            sameColor.push_back(idx);
        }
    }
    
    if (!sameColor.empty()) {
        int randomIndex = Utils::getRandomInt(0, sameColor.size() - 1);
        return sameColor[randomIndex];
    }
    
    // Fallback: gioca una carta a caso
    int randomIndex = Utils::getRandomInt(0, validIndices.size() - 1);
    return validIndices[randomIndex];
}

bool AI::hasWildCard() const {
    for (const auto& card : getHand()) {
        if (card->isWild()) {
            return true;
        }
    }
    return false;
}

bool AI::hasSpecialCard() const {
    for (const auto& card : getHand()) {
        if (card->isSpecial()) {
            return true;
        }
    }
    return false;
}

CardColor AI::getMostFrequentColor() const {
    std::map<CardColor, int> colorCount;
    
    for (const auto& card : getHand()) {
        if (!card->isWild()) {
            colorCount[card->getColor()]++;
        }
    }
    
    CardColor mostFrequent = CardColor::RED;
    int maxCount = 0;
    
    for (const auto& [color, count] : colorCount) {
        if (count > maxCount) {
            maxCount = count;
            mostFrequent = color;
        }
    }
    
    return mostFrequent;
}

int AI::chooseCard(const Card& topCard) {
    switch (strategy) {
        case AIStrategy::RANDOM:
            return selectCardRandom(topCard);
        case AIStrategy::AGGRESSIVE:
            return selectCardAggressive(topCard);
        case AIStrategy::SMART:
            return selectCardSmart(topCard);
        default:
            return selectCardRandom(topCard);
    }
}

CardColor AI::chooseWildColor() const {
    // L'IA sceglie il colore più frequente in mano
    return getMostFrequentColor();
}

AIStrategy AI::getStrategy() const {
    return strategy;
}
