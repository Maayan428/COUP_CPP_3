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

public:
    Game();

    void addPlayer(Player* player);
    Player& currentPlayer();
    void nextTurn();

    std::vector<std::string> activePlayers() const;
    std::string winner() const;

    int getTurnIndexOf(Player* p) const;
    bool isGameOver() const;

    void setExtraTurn(Player* p);
    bool isPlayerSanctioned(Player* p) const;
    void applySanction(Player* p);

     // Action attempts with a possibility to be blocked
     void attemptBribe(Player* actor);
     void attemptTax(Player* actor);
     void attemptCoup(Player* actor, Player& target);
};

#endif // GAME_HPP