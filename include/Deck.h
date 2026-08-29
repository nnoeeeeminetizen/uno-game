#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <vector>
#include <stdexcept>

// Exception per il mazzo
class DeckException : public std::runtime_error {
public:
    explicit DeckException(const std::string& message) : std::runtime_error(message) {}
};

class Deck {
private:
    Card** cards;              // Puntatore dinamico a array di puntatori
    int cardsCount;
    int cardsCapacity;
    
    Card** discardPile;        // Pile di scarto
    int discardCount;
    int discardCapacity;
    
    // Utility
    void initializeDeck();
    void shuffle();
    void resizeCardsArray();
    void resizeDiscardArray();

public:
    Deck();
    ~Deck();
    
    // Operazioni sul mazzo
    Card* drawCard();          // Restituisce puntatore a carta
    void discardCard(Card* card);
    Card* getTopCard() const;
    void resetDeck();
    
    // Getters
    int getRemainingCards() const;
    int getDiscardPileSize() const;
    Card** getDiscardPile() const;
    
    // Debug
    void printDeckInfo() const;
};

#endif // DECK_H
