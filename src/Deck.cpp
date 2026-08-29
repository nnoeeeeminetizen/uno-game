#include "Deck.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>

Deck::Deck() {
    initializeDeck();
    shuffle();
}

void Deck::initializeDeck() {
    // Per ogni colore (escluso WILD)
    for (int c = 0; c < 4; c++) {
        CardColor color = static_cast<CardColor>(c);
        
        // Carte numero 0-9
        for (int i = 0; i < 10; i++) {
            CardType type = static_cast<CardType>(i);
            cards.push_back(std::make_shared<Card>(color, type, i));
            
            // Aggiungi duplicati (tranne lo 0)
            if (i != 0) {
                cards.push_back(std::make_shared<Card>(color, type, i));
            }
        }
        
        // Carte speciali (2 per tipo)
        cards.push_back(std::make_shared<Card>(color, CardType::SKIP, -1));
        cards.push_back(std::make_shared<Card>(color, CardType::SKIP, -1));
        
        cards.push_back(std::make_shared<Card>(color, CardType::REVERSE, -1));
        cards.push_back(std::make_shared<Card>(color, CardType::REVERSE, -1));
        
        cards.push_back(std::make_shared<Card>(color, CardType::DRAW_TWO, -1));
        cards.push_back(std::make_shared<Card>(color, CardType::DRAW_TWO, -1));
    }
    
    // Carte WILD (4 WILD + 4 WILD_DRAW_FOUR)
    for (int i = 0; i < 4; i++) {
        cards.push_back(std::make_shared<Card>(CardColor::WILD, CardType::WILD, -1));
        cards.push_back(std::make_shared<Card>(CardColor::WILD, CardType::WILD_DRAW_FOUR, -1));
    }
}

void Deck::shuffle() {
    std::shuffle(cards.begin(), cards.end(), 
                 std::default_random_engine(static_cast<unsigned>(std::time(nullptr))));
}

std::shared_ptr<Card> Deck::drawCard() {
    if (cards.empty()) {
        // Se non ci sono carte nel mazzo, ricostituiscilo dal discard pile
        if (discardPile.empty()) {
            std::cout << "ERROR: No cards available!" << std::endl;
            return nullptr;
        }
        
        // Sposta tutte le carte dal discard pile al mazzo tranne l'ultima
        while (discardPile.size() > 1) {
            cards.push_back(discardPile.back());
            discardPile.pop_back();
        }
        shuffle();
    }
    
    auto card = cards.back();
    cards.pop_back();
    return card;
}

void Deck::discardCard(std::shared_ptr<Card> card) {
    discardPile.push_back(card);
}

std::shared_ptr<Card> Deck::getTopCard() const {
    if (discardPile.empty()) {
        return nullptr;
    }
    return discardPile.back();
}

void Deck::resetDeck() {
    cards.clear();
    discardPile.clear();
    initializeDeck();
    shuffle();
}

int Deck::getRemainingCards() const {
    return cards.size();
}

int Deck::getDiscardPileSize() const {
    return discardPile.size();
}

std::vector<std::shared_ptr<Card>> Deck::getDiscardPile() const {
    return discardPile;
}

void Deck::printDeckInfo() const {
    std::cout << "Deck Info:" << std::endl;
    std::cout << "  Cards remaining: " << getRemainingCards() << std::endl;
    std::cout << "  Discard pile: " << getDiscardPileSize() << std::endl;
    if (!discardPile.empty()) {
        std::cout << "  Top card: " << getTopCard()->toString() << std::endl;
    }
}
