#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <vector>
#include <memory>

class Deck {
private:
    std::vector<std::shared_ptr<Card>> cards;
    std::vector<std::shared_ptr<Card>> discardPile;
    
    // Utility
    void initializeDeck();
    void shuffle();

public:
    Deck();
    
    // Operazioni sul mazzo
    std::shared_ptr<Card> drawCard();
    void discardCard(std::shared_ptr<Card> card);
    std::shared_ptr<Card> getTopCard() const;
    void resetDeck();
    
    // Getters
    int getRemainingCards() const;
    int getDiscardPileSize() const;
    std::vector<std::shared_ptr<Card>> getDiscardPile() const;
    
    // Debug
    void printDeckInfo() const;
};

#endif // DECK_H
