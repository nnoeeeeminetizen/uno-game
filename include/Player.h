#ifndef PLAYER_H
#define PLAYER_H

#include "Card.h"
#include <string>
#include <stdexcept>

// Exception per il giocatore
class PlayerException : public std::runtime_error {
public:
    explicit PlayerException(const std::string& message) : std::runtime_error(message) {}
};

class Player {
private:
    std::string name;
    Card** hand;               // Array dinamico di puntatori a carte
    int handSize;
    int handCapacity;
    int gamesWon;
    int gamesLost;
    bool isAI;
    int playerID;
    
    // Utility
    void resizeHand();

public:
    Player(const std::string& name, int id, bool isAI = false);
    virtual ~Player();
    
    // Gestione mano
    void addCard(Card* card);
    Card* playCard(int cardIndex);
    void removeCard(int cardIndex);
    bool hasCards() const;
    int getHandSize() const;
    
    // Getters
    std::string getName() const;
    int getPlayerID() const;
    bool getIsAI() const;
    Card** getHand() const;
    int getGamesWon() const;
    int getGamesLost() const;
    
    // Statistiche
    void incrementWins();
    void incrementLosses();
    void resetStats();
    
    // Validazione
    bool canPlayCard(const Card& topCard);
    int* getValidCardIndices(const Card& topCard, int& count);
    
    // Virtual per polimorfismo
    virtual int chooseCard(const Card& topCard);
    virtual CardColor chooseWildColor() const;
    
    // Debug
    void printHand() const;
    void printStats() const;
};

#endif // PLAYER_H
