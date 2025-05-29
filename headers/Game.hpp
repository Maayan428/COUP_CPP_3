//maayan428@gmail.com

#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <map>

class Player; //Allows me to use pointers to Player objects in the Game class.

class Game {
private:
    std::vector<Player*> players;
    std::map <Player*, int> sanctionedUntilNextTurn;
    Player* extraTurnPlayer;
    int currentTurnIndex;
    Player* lastArrested = nullptr; 

public:
    Game();
    ~Game();
    void run();

    void addPlayer(Player* player);
    Player& currentPlayer();
    void nextTurn();

    Player* getLastArrested() const { return lastArrested; }
    void setLastArrested(Player* p) { lastArrested = p; }

    const std::vector<Player*>& getPlayers() const;
    std::vector<std::string> activePlayers() const;
    std::string winner() const;

    int getTurnIndexOf(Player* p) const;
    bool isGameOver() const;

    void setExtraTurn(Player* p);
    bool isPlayerSanctioned(Player* p) const;
    void applySanction(Player* p);

    void initializePlayers();
    void mainGameLoop();
    void displayActions(const Player& player);
    void handleAction(int choice, Player& player);
    bool hasValidTargets(Player* current) const;
    Player* selectTarget(Player* current);
    Player* createRandomRole(const std::string& name); 

     // Action attempts with a possibility to be blocked
     void attemptBribe(Player* actor);
     void attemptTax(Player* actor);
     void attemptCoup(Player* actor, Player& target);
};

#endif // GAME_HPP