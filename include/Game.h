#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Deck.h"
#include "AI.h"
#include <vector>
#include <memory>
#include <string>

class Game {
private:
    std::vector<std::shared_ptr<Player>> players;
    std::shared_ptr<Deck> deck;
    int currentPlayerIndex;
    bool gameActive;
    bool reverseDirection;  // True = senso orario inverso
    std::string savedGamePath;
    
    // Gestione turni
    void nextTurn();
    void previousTurn();
    void skipTurn();
    
    // Gestione carte speciali
    void handleSkip();
    void handleReverse();
    void handleDrawTwo();
    void handleWild(std::shared_ptr<Player> player);
    void handleWildDrawFour(std::shared_ptr<Player> player);
    
    // Utility
    void displayGameState() const;
    void distributeInitialCards();
    Player* getCurrentPlayer() const;
    Player* getNextPlayer() const;
    int getNextPlayerIndex() const;
    
public:
    Game();
    ~Game();
    
    // Inizializzazione e gestione partita
    void initializeGame(int numAIPlayers = 3);
    void playRound();
    void playGame();
    
    // Input giocatore umano
    bool humanPlayerTurn();
    
    // Turno IA
    void aiPlayerTurn();
    
    // Controllo stato
    bool isGameActive() const;
    std::shared_ptr<Player> getWinner() const;
    
    // Persistenza
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
    
    // Debug e statistiche
    void printGameState() const;
    void printPlayerStats() const;
    void resetScores();
};

#endif // GAME_H
