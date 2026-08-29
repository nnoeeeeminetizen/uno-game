#ifndef CARD_H
#define CARD_H

#include <string>
#include <iostream>
#include <stdexcept>

enum class CardColor {
    RED,
    YELLOW,
    GREEN,
    BLUE,
    WILD
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

// Exception personalizzate
class CardException : public std::runtime_error {
public:
    explicit CardException(const std::string& message) : std::runtime_error(message) {}
};

class Card {
private:
    CardColor color;
    CardType type;
    int number;

public:
    Card(CardColor color, CardType type, int number = -1);
    ~Card();
    
    // Getters
    CardColor getColor() const;
    CardType getType() const;
    int getNumber() const;
    
    // Utility
    std::string toString() const;
    bool isWild() const;
    bool isSpecial() const;
    bool isNumber() const;
    
    // Validazione: controlla se questa carta puo' essere giocata su un'altra
    bool canPlayOn(const Card& topCard) const;
};

#endif // CARD_H
