#include "AI.h"
#include "Utils.h"
#include <algorithm>
#include <map>
#include <iostream>

AI::AI(const std::string& name, int id, AIStrategy strategy)
    : Player(name, id, true), strategy(strategy) {}

AI::~AI() {}

int AI::selectCardRandom(const Card& topCard) {
    try {
        int validCount = 0;
        int* validIndices = getValidCardIndices(topCard, validCount);
        
        if (validCount == 0) {
            delete[] validIndices;
            return -1;
        }
        
        int randomIdx = Utils::getRandomInt(0, validCount - 1);
        int result = validIndices[randomIdx];
        delete[] validIndices;
        
        return result;
    } catch (const std::exception& e) {
        throw PlayerException("Error in selectCardRandom: " + std::string(e.what()));
    }
}

int AI::selectCardAggressive(const Card& topCard) {
    try {
        int validCount = 0;
        int* validIndices = getValidCardIndices(topCard, validCount);
        
        if (validCount == 0) {
            delete[] validIndices;
            return -1;
        }
        
        // Preferisci carte speciali
        for (int i = 0; i < validCount; i++) {
            int idx = validIndices[i];
            if (getHand()[idx]->isSpecial()) {
                delete[] validIndices;
                return idx;
            }
        }
        
        // Se non ci sono carte speciali, gioca una carta a caso
        int randomIdx = Utils::getRandomInt(0, validCount - 1);
        int result = validIndices[randomIdx];
        delete[] validIndices;
        
        return result;
    } catch (const std::exception& e) {
        throw PlayerException("Error in selectCardAggressive: " + std::string(e.what()));
    }
}

int AI::selectCardSmart(const Card& topCard) {
    try {
        int validCount = 0;
        int* validIndices = getValidCardIndices(topCard, validCount);
        
        if (validCount == 0) {
            delete[] validIndices;
            return -1;
        }
        
        // Priorità 1: Se hai tante carte, preferisci le speciali
        if (getHandSize() > 5) {
            for (int i = 0; i < validCount; i++) {
                int idx = validIndices[i];
                if (getHand()[idx]->isSpecial()) {
                    delete[] validIndices;
                    return idx;
                }
            }
        }
        
        // Priorità 2: Mantieni la diversità di colori
        CardColor topColor = topCard.getColor();
        int* sameColorIndices = new int[validCount];
        int sameColorCount = 0;
        
        for (int i = 0; i < validCount; i++) {
            int idx = validIndices[i];
            if (getHand()[idx]->getColor() == topColor) {
                sameColorIndices[sameColorCount++] = idx;
            }
        }
        
        int result;
        if (sameColorCount > 0) {
            int randomIdx = Utils::getRandomInt(0, sameColorCount - 1);
            result = sameColorIndices[randomIdx];
        } else {
            // Fallback: gioca una carta a caso
            int randomIdx = Utils::getRandomInt(0, validCount - 1);
            result = validIndices[randomIdx];
        }
        
        delete[] validIndices;
        delete[] sameColorIndices;
        
        return result;
    } catch (const std::exception& e) {
        throw PlayerException("Error in selectCardSmart: " + std::string(e.what()));
    }
}

bool AI::hasWildCard() const {
    for (int i = 0; i < getHandSize(); i++) {
        if (getHand()[i] != nullptr && getHand()[i]->isWild()) {
            return true;
        }
    }
    return false;
}

bool AI::hasSpecialCard() const {
    for (int i = 0; i < getHandSize(); i++) {
        if (getHand()[i] != nullptr && getHand()[i]->isSpecial()) {
            return true;
        }
    }
    return false;
}

CardColor AI::getMostFrequentColor() const {
    std::map<CardColor, int> colorCount;
    
    for (int i = 0; i < getHandSize(); i++) {
        if (getHand()[i] != nullptr && !getHand()[i]->isWild()) {
            colorCount[getHand()[i]->getColor()]++;
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
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Error in AI::chooseCard: " << e.what() << std::endl;
        return -1;
    }
}

CardColor AI::chooseWildColor() const {
    return getMostFrequentColor();
}

AIStrategy AI::getStrategy() const {
    return strategy;
}
