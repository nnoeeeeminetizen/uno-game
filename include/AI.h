#ifndef AI_H
#define AI_H

#include "Player.h"
#include <memory>

enum class AIStrategy {
    RANDOM,      // Gioca carte a caso
    AGGRESSIVE,  // Preferisce carte speciali
    SMART        // Strategia avanzata
};

class AI : public Player {
private:
    AIStrategy strategy;
    
    // Strategie di scelta
    int selectCardRandom(const Card& topCard);
    int selectCardAggressive(const Card& topCard);
    int selectCardSmart(const Card& topCard);
    
    // Utility per strategie
    bool hasWildCard() const;
    bool hasSpecialCard() const;
    CardColor getMostFrequentColor() const;

public:
    AI(const std::string& name, int id, AIStrategy strategy = AIStrategy::RANDOM);
    
    // Decisione di gioco
    int chooseCard(const Card& topCard);
    CardColor chooseWildColor() const;
    
    // Getters
    AIStrategy getStrategy() const;
};

#endif // AI_H
