#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream>

enum class CardColor {
    RED,
    YELLOW,
    GREEN,
    BLUE,
    WILD  // Per carte Wild
};

enum class CardType {
    NUMBER_0, NUMBER_1, NUMBER_2, NUMBER_3, NUMBER_4,
    NUMBER_5, NUMBER_6, NUMBER_7, NUMBER_8, NUMBER_9,
    SKIP,
    REVERSE,
    DRAW_TWO,
    WILD,
    WILD_DRAW_FOUR
};

class Card {
private:
    CardColor color;
    CardType type;
    int number;  // Per carte numero

public:
    Card(CardColor color, CardType type, int number = -1);
    
    // Getters
    CardColor getColor() const;
    CardType getType() const;
    int getNumber() const;
    
    // Utility
    std::string toString() const;
    bool isWild() const;
    bool isSpecial() const;
    bool isNumber() const;
    
    // Validazione: controlla se questa carta può essere giocata su un'altra
    bool canPlayOn(const Card& topCard) const;
};

#endif // CARD_H
