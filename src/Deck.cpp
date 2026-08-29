#include "Deck.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>
#include <cstring>

Deck::Deck() : cardsCount(0), cardsCapacity(108), discardCount(0), discardCapacity(50) {
    try {
        // Alloca memoria per il mazzo
        cards = new Card*[cardsCapacity];
        if (!cards) {
            throw DeckException("Failed to allocate memory for cards array");
        }
        
        // Alloca memoria per il discard pile
        discardPile = new Card*[discardCapacity];
        if (!discardPile) {
            delete[] cards;
            throw DeckException("Failed to allocate memory for discard pile");
        }
        
        initializeDeck();
        shuffle();
    } catch (const std::bad_alloc& e) {
        throw DeckException("Memory allocation error in Deck constructor");
    }
}

Deck::~Deck() {
    try {
        // Dealloca tutte le carte
        for (int i = 0; i < cardsCount; i++) {
            if (cards[i] != nullptr) {
                delete cards[i];
                cards[i] = nullptr;
            }
        }
        
        // Dealloca gli array
        if (cards != nullptr) {
            delete[] cards;
            cards = nullptr;
        }
        
        if (discardPile != nullptr) {
            delete[] discardPile;
            discardPile = nullptr;
        }
        
        cardsCount = 0;
        discardCount = 0;
    } catch (const std::exception& e) {
        std::cerr << "Error during Deck destruction: " << e.what() << std::endl;
    }
}

void Deck::initializeDeck() {
    try {
        // Per ogni colore (escluso WILD)
        for (int c = 0; c < 4; c++) {
            CardColor color = static_cast<CardColor>(c);
            
            // Carte numero 0-9
            for (int i = 0; i < 10; i++) {
                CardType type = static_cast<CardType>(i);
                cards[cardsCount++] = new Card(color, type, i);
                
                // Aggiungi duplicati (tranne lo 0)
                if (i != 0) {
                    cards[cardsCount++] = new Card(color, type, i);
                }
            }
            
            // Carte speciali (2 per tipo)
            cards[cardsCount++] = new Card(color, CardType::SKIP, -1);
            cards[cardsCount++] = new Card(color, CardType::SKIP, -1);
            
            cards[cardsCount++] = new Card(color, CardType::REVERSE, -1);
            cards[cardsCount++] = new Card(color, CardType::REVERSE, -1);
            
            cards[cardsCount++] = new Card(color, CardType::DRAW_TWO, -1);
            cards[cardsCount++] = new Card(color, CardType::DRAW_TWO, -1);
        }
        
        // Carte WILD (4 WILD + 4 WILD_DRAW_FOUR)
        for (int i = 0; i < 4; i++) {
            cards[cardsCount++] = new Card(CardColor::WILD, CardType::WILD, -1);
            cards[cardsCount++] = new Card(CardColor::WILD, CardType::WILD_DRAW_FOUR, -1);
        }
    } catch (const CardException& e) {
        throw DeckException("Failed to create cards: " + std::string(e.what()));
    }
}

void Deck::shuffle() {
    try {
        // Fisher-Yates shuffle algorithm
        for (int i = cardsCount - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            // Scambia
            Card* temp = cards[i];
            cards[i] = cards[j];
            cards[j] = temp;
        }
    } catch (const std::exception& e) {
        throw DeckException("Error during shuffle: " + std::string(e.what()));
    }
}

void Deck::resizeCardsArray() {
    try {
        if (cardsCount >= cardsCapacity) {
            int newCapacity = cardsCapacity * 2;
            Card** newCards = new Card*[newCapacity];
            if (!newCards) {
                throw DeckException("Failed to allocate memory for resized cards array");
            }
            
            // Copia i puntatori
            for (int i = 0; i < cardsCount; i++) {
                newCards[i] = cards[i];
            }
            
            delete[] cards;
            cards = newCards;
            cardsCapacity = newCapacity;
        }
    } catch (const std::bad_alloc& e) {
        throw DeckException("Memory allocation error during cards array resize");
    }
}

void Deck::resizeDiscardArray() {
    try {
        if (discardCount >= discardCapacity) {
            int newCapacity = discardCapacity * 2;
            Card** newDiscard = new Card*[newCapacity];
            if (!newDiscard) {
                throw DeckException("Failed to allocate memory for resized discard pile");
            }
            
            for (int i = 0; i < discardCount; i++) {
                newDiscard[i] = discardPile[i];
            }
            
            delete[] discardPile;
            discardPile = newDiscard;
            discardCapacity = newCapacity;
        }
    } catch (const std::bad_alloc& e) {
        throw DeckException("Memory allocation error during discard array resize");
    }
}

Card* Deck::drawCard() {
    try {
        if (cardsCount == 0) {
            // Ricostituisci il mazzo dal discard pile
            if (discardCount <= 1) {
                throw DeckException("No cards available to draw!");
            }
            
            // Sposta tutte le carte dal discard pile al mazzo tranne l'ultima
            cardsCount = 0;
            for (int i = 0; i < discardCount - 1; i++) {
                cards[cardsCount++] = discardPile[i];
            }
            
            // Mantieni solo l'ultima carta nel discard pile
            discardPile[0] = discardPile[discardCount - 1];
            discardCount = 1;
            
            shuffle();
        }
        
        if (cardsCount > 0) {
            Card* card = cards[cardsCount - 1];
            cardsCount--;
            return card;
        }
        
        throw DeckException("Failed to draw card");
    } catch (const DeckException& e) {
        throw;
    } catch (const std::exception& e) {
        throw DeckException("Error drawing card: " + std::string(e.what()));
    }
}

void Deck::discardCard(Card* card) {
    try {
        if (card == nullptr) {
            throw DeckException("Cannot discard null card");
        }
        
        resizeDiscardArray();
        discardPile[discardCount++] = card;
    } catch (const std::exception& e) {
        throw DeckException("Error discarding card: " + std::string(e.what()));
    }
}

Card* Deck::getTopCard() const {
    if (discardCount == 0) {
        return nullptr;
    }
    return discardPile[discardCount - 1];
}

void Deck::resetDeck() {
    try {
        // Dealloca le carte
        for (int i = 0; i < cardsCount; i++) {
            if (cards[i] != nullptr) {
                delete cards[i];
                cards[i] = nullptr;
            }
        }
        
        cardsCount = 0;
        discardCount = 0;
        
        initializeDeck();
        shuffle();
    } catch (const std::exception& e) {
        throw DeckException("Error resetting deck: " + std::string(e.what()));
    }
}

int Deck::getRemainingCards() const {
    return cardsCount;
}

int Deck::getDiscardPileSize() const {
    return discardCount;
}

Card** Deck::getDiscardPile() const {
    return discardPile;
}

void Deck::printDeckInfo() const {
    std::cout << "Deck Info:" << std::endl;
    std::cout << "  Cards remaining: " << getRemainingCards() << std::endl;
    std::cout << "  Discard pile: " << getDiscardPileSize() << std::endl;
    if (discardCount > 0 && getTopCard() != nullptr) {
        std::cout << "  Top card: " << getTopCard()->toString() << std::endl;
    }
}
