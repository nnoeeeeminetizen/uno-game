#ifndef PLAYER_H
#define PLAYER_H

#include "Card.h"
#include <vector>
#include <string>
#include <memory>

class Player {
private:
    std::string name;
    std::vector<std::shared_ptr<Card>> hand;
    int gamesWon;
    int gamesLost;
    bool isAI;
    int playerID;  // 0 = umano, 1-3 = IA

public:
    Player(const std::string& name, int id, bool isAI = false);
    
    // Gestione mano
    void addCard(std::shared_ptr<Card> card);
    std::shared_ptr<Card> playCard(int cardIndex);
    void removeCard(int cardIndex);
    bool hasCards() const;
    int getHandSize() const;
    
    // Getters
    std::string getName() const;
    int getPlayerID() const;
    bool getIsAI() const;
    std::vector<std::shared_ptr<Card>> getHand() const;
    int getGamesWon() const;
    int getGamesLost() const;
    
    // Statistiche
    void incrementWins();
    void incrementLosses();
    void resetStats();
    
    // Validazione
    bool canPlayCard(const Card& topCard);
    std::vector<int> getValidCardIndices(const Card& topCard);
    
    // Debug
    void printHand() const;
    void printStats() const;
};

#endif // PLAYER_H
