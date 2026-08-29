#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <cstring>

// Messaggi nel protocollo di comunicazione
enum class MessageType {
    // Client -> Server
    JOIN_GAME = 0,           // Richiedi di entrare in una partita
    PLAY_CARD = 1,           // Gioca una carta
    DRAW_CARD = 2,           // Pesca una carta
    CHOOSE_COLOR = 3,        // Scegli colore per Wild
    QUIT_GAME = 4,           // Esci dalla partita
    
    // Server -> Client
    GAME_START = 10,         // La partita inizia
    PLAYER_TURN = 11,        // È il tuo turno
    CARD_PLAYED = 12,        // Un giocatore ha giocato una carta
    CARD_DRAWN = 13,         // Un giocatore ha pescato una carta
    GAME_STATE = 14,         // Stato della partita
    GAME_END = 15,           // La partita è finita
    ERROR_MSG = 20           // Messaggio di errore
};

// Struttura per la comunicazione
struct Message {
    MessageType type;
    char playerName[32];
    char payload[256];        // Dati aggiuntivi (JSON)
    int playerId;
    
    Message() : type(MessageType::ERROR_MSG), playerId(-1) {
        std::memset(playerName, 0, sizeof(playerName));
        std::memset(payload, 0, sizeof(payload));
    }
    
    Message(MessageType t, const std::string& name, const std::string& data, int id = -1)
        : type(t), playerId(id) {
        std::strncpy(playerName, name.c_str(), sizeof(playerName) - 1);
        std::strncpy(payload, data.c_str(), sizeof(payload) - 1);
    }
};

// Costanti di rete
const int DEFAULT_PORT = 5555;
const int MAX_PLAYERS = 4;
const int BUFFER_SIZE = 512;
const int BACKLOG = 5;

#endif // PROTOCOL_H
