# UNO Card Game - C++ Implementation

## Descrizione

Un'implementazione completa del gioco di carte **UNO** in C++17, sviluppata secondo i standard di programmazione orientata agli oggetti con gestione dinamica della memoria, controllo delle eccezioni e architettura client-server.

**1 Giocatore Umano + 3 Giocatori IA** con strategie diverse (Random, Aggressive, Smart).

---

## Caratteristiche Principali

### Core Game
- ✅ **Logica completa di UNO** - Carte numero, speciali (Skip, Reverse, Draw Two), Wild
- ✅ **Gestione turni** - Ordine circolare con supporto a Reverse
- ✅ **Validazione mosse** - Controllo delle regole di gioco
- ✅ **3 Strategie IA**:
  - *Random*: Gioca carte casuali
  - *Aggressive*: Preferisce carte speciali
  - *Smart*: Strategia avanzata con controllo della mano

### Programmazione Avanzata
- ✅ **Gestione Dinamica della Memoria** - Puntatori grezzi con `new`/`delete`
- ✅ **Controllo Eccezioni** - Try/catch per gestione errori
- ✅ **Classi e Oggetti** - Ereditarietà (AI extends Player)
- ✅ **Strutture Dati Dinamiche** - Vector, liste interne
- ✅ **Enum e Type Safety** - CardColor, CardType

### Funzionalità Estese (Roadmap)
- 🔄 **Client-Server Architecture** - Multiplayer online
- 🔄 **Database SQLite** - Persistenza statistiche giocatori
- 🔄 **Interfaccia Grafica** - Ncurses o SDL2
- 🔄 **Salvataggio/Caricamento Partite** - Serializzazione stato gioco

---

## Struttura del Progetto

```
uno-game/
├── CMakeLists.txt           # Build configuration
├── include/
│   ├── Card.h              # Definizione carte
│   ├── Deck.h              # Gestione mazzo
│   ├── Player.h            # Classe giocatore
│   ├── AI.h                # Classe IA (extends Player)
│   ├── Game.h              # Logica principale
│   └── Utils.h             # Funzioni utility
├── src/
│   ├── main.cpp            # Entry point
│   ├── Card.cpp            # Implementazione carte
│   ├── Deck.cpp            # Implementazione mazzo
│   ├── Player.cpp          # Implementazione giocatore
│   ├── AI.cpp              # Implementazione IA
│   ├── Game.cpp            # Implementazione logica gioco
│   └── Utils.cpp           # Implementazione utility
└── README.md               # Questo file
```

---

## Compilazione e Esecuzione

### Requisiti
- **Compilatore**: GCC/Clang con supporto C++17
- **Build System**: CMake 3.10+
- **Dipendenze**: Nessuna (uso standard library)

### Compilare

```bash
# Clona il repository
git clone https://github.com/nnoeeeeminetizen/uno-game.git
cd uno-game

# Crea directory di build
mkdir build
cd build

# Configura con CMake
cmake ..

# Compila
make

# Esegui
./uno-game
```

### Su Windows (Visual Studio)

```bash
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
.\Release\uno-game.exe
```

---

## Come Giocare

### Menu Principale
```
1. Start New Game       - Inizia una nuova partita
2. Load Saved Game      - Carica una partita precedente
3. View Statistics      - Visualizza statistiche
4. Exit                 - Esci
```

### Durante il Gioco

1. **Visualizza la tua mano** - Ogni carta ha un indice [0], [1], ecc.
2. **Scegli una carta valida** - Il gioco verifica se è compatibile con la carta in cima
3. **Carte Wild** - Scegli il colore preferito
4. **Pesca se necessario** - Se non hai carte valide, pesca automaticamente

### Regole di UNO

| Carta | Effetto |
|-------|----------|
| **Numero** | Gioca se stesso colore o numero |
| **Skip** | Salta il turno del prossimo giocatore |
| **Reverse** | Cambia direzione di gioco |
| **Draw Two** | Prossimo giocatore pesca 2 carte |
| **Wild** | Gioca sempre, scegli colore |
| **Wild Draw Four** | Come Wild, prossimo pesca 4 carte |

---

## Architettura e Classi

### Card
```cpp
class Card {
    CardColor color;
    CardType type;
    int number;
    
public:
    bool canPlayOn(const Card& topCard) const;
    bool isWild() const;
    bool isSpecial() const;
    std::string toString() const;
};
```

### Deck
```cpp
class Deck {
    std::vector<Card*> cards;           // Puntatori a carte
    std::vector<Card*> discardPile;
    
public:
    Card* drawCard();                   // Restituisce puntatore
    void discardCard(Card* card);
    Card* getTopCard() const;
};
```

### Player
```cpp
class Player {
    std::string name;
    std::vector<Card*> hand;            // Puntatori a carte in mano
    int gamesWon, gamesLost;
    bool isAI;
    
public:
    void addCard(Card* card);
    Card* playCard(int cardIndex);
    std::vector<int> getValidCardIndices(const Card& topCard);
    virtual int chooseCard(const Card& topCard);  // Virtual per polimorfismo
};
```

### AI (extends Player)
```cpp
class AI : public Player {
    AIStrategy strategy;  // RANDOM, AGGRESSIVE, SMART
    
public:
    int chooseCard(const Card& topCard) override;  // Override polimorfismo
    CardColor chooseWildColor() const;
};
```

### Game
```cpp
class Game {
    std::vector<Player*> players;       // Puntatori ai giocatori
    Deck* deck;                         // Puntatore al mazzo
    
public:
    void initializeGame(int numAIPlayers);
    void playGame();
    bool humanPlayerTurn();
    void aiPlayerTurn();
};
```

---

## Gestione della Memoria

### Allocazione Dinamica

```cpp
// Allocazione
Card* card = new Card(CardColor::RED, CardType::NUMBER_5, 5);
Player* player = new Player("Alice", 0, false);
AI* ai = new AI("Bot-1", 1, AIStrategy::SMART);
Deck* deck = new Deck();

// Utilizzo
player->addCard(card);

// Deallocazione
delete card;
delete player;
delete ai;
delete deck;
```

### Gestione Eccezioni

```cpp
try {
    Game game;
    game.initializeGame(3);
    game.playGame();
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
```

---

## Strategia IA

### Random Strategy
- Sceglie una carta valida a caso
- Buona per test e debugging

### Aggressive Strategy
- Preferisce carte speciali (Skip, Reverse, Draw Two, Wild)
- Tenta di sabotare i giocatori avversari

### Smart Strategy
- Mantiene diversità di colori
- Gioca strategicamente in base alla situazione
- Se ha tante carte, usa speciali per ridurre la mano
- Sceglie il colore più frequente per Wild

---

## Funzionalità Pianificate

### Phase 1: Console Base ✅
- [x] Logica core di UNO
- [x] 3 Strategie IA
- [x] Validazione mosse
- [x] Gestione turni

### Phase 2: Persistenza 🔄
- [ ] Salvataggio partite (serializzazione JSON/Binary)
- [ ] Database SQLite per statistiche
- [ ] Caricamento partite

### Phase 3: Multiplayer 🔄
- [ ] Architettura Client-Server (Socket/Boost.Asio)
- [ ] Comunicazione di rete
- [ ] Sincronizzazione stato gioco

### Phase 4: Interfaccia 🔄
- [ ] Ncurses per TUI avanzata
- [ ] SDL2 per grafica 2D (opzionale)

---

## Testing

```bash
# Compila con flag di debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Esegui con valgrind (memory leak detection)
valgrind --leak-check=full ./uno-game
```

---

## Docente Info

Progetto sviluppato seguendo i criteri di valutazione:
- ✅ Puntatori e gestione dinamica della memoria
- ✅ Strutture dati dinamiche
- ✅ Controllo delle eccezioni
- ✅ Classi e oggetti C++ (ereditarietà, polimorfismo)
- 🔄 Estensioni di dominio (Client-Server, Database, GUI)

---

## Autore

**nnoeeeeminetizen**

---

## Licenza

MIT License - Vedi LICENSE file per dettagli.

---

## Contatti e Support

Per problemi o suggerimenti, apri un **Issue** su GitHub!

👉 https://github.com/nnoeeeeminetizen/uno-game/issues
