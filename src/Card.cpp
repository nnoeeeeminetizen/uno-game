#include "Card.h"

Card::Card(CardColor color, CardType type, int number)
    : color(color), type(type), number(number) {
    if (isNumber() && (number < 0 || number > 9)) {
        throw CardException("Invalid card number: " + std::to_string(number));
    }
}

Card::~Card() {}

CardColor Card::getColor() const {
    return color;
}

CardType Card::getType() const {
    return type;
}

int Card::getNumber() const {
    return number;
}

bool Card::isWild() const {
    return type == CardType::WILD || type == CardType::WILD_DRAW_FOUR;
}

bool Card::isSpecial() const {
    return type == CardType::SKIP || type == CardType::REVERSE || 
           type == CardType::DRAW_TWO || isWild();
}

bool Card::isNumber() const {
    return type >= CardType::NUMBER_0 && type <= CardType::NUMBER_9;
}

bool Card::canPlayOn(const Card& topCard) const {
    // Carte Wild si possono sempre giocare
    if (isWild()) {
        return true;
    }
    
    // Se la carta in cima è Wild, puoi giocare qualsiasi carta dello stesso colore
    if (topCard.isWild()) {
        return this->color == topCard.color;
    }
    
    // Stesso colore
    if (this->color == topCard.color) {
        return true;
    }
    
    // Stesso tipo (per carte numero)
    if (this->isNumber() && topCard.isNumber()) {
        return this->number == topCard.number;
    }
    
    // Stesso tipo (per carte speciali)
    if (this->type == topCard.type) {
        return true;
    }
    
    return false;
}

std::string Card::toString() const {
    std::string colorStr;
    std::string typeStr;
    
    // Colore
    switch (color) {
        case CardColor::RED: colorStr = "RED"; break;
        case CardColor::YELLOW: colorStr = "YELLOW"; break;
        case CardColor::GREEN: colorStr = "GREEN"; break;
        case CardColor::BLUE: colorStr = "BLUE"; break;
        case CardColor::WILD: colorStr = "WILD"; break;
    }
    
    // Tipo
    if (isNumber()) {
        typeStr = std::to_string(number);
    } else {
        switch (type) {
            case CardType::SKIP: typeStr = "SKIP"; break;
            case CardType::REVERSE: typeStr = "REVERSE"; break;
            case CardType::DRAW_TWO: typeStr = "DRAW_TWO"; break;
            case CardType::WILD: typeStr = "WILD"; break;
            case CardType::WILD_DRAW_FOUR: typeStr = "WILD_DRAW_FOUR"; break;
            default: typeStr = "UNKNOWN"; break;
        }
    }
    
    return colorStr + " " + typeStr;
}
