//maayan428@gmail.com

#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Game.hpp"
#include "Player.hpp"
#include "Role.hpp"

using namespace std;

int main(){

    Game game{};

    cout << "\n========== START COUP GAME ==========\n" << endl;

    Governor* governor = new Governor("Lior");
    Spy* spy = new Spy("Amit");
    Baron* baron = new Baron("Noam");
    General* general = new General("Gil");
    Judge* judge = new Judge("Eden");

    game.addPlayer(governor);
    game.addPlayer(spy);
    game.addPlayer(baron);
    game.addPlayer(general);
    game.addPlayer(judge);

    cout << "\n========== PLAYERS: ==========\n" << endl;

    vector<Player*> players = game.getPlayers();
    
    for(Player* p : players){
        cout << p->getName() << endl;
    }

    cout << "\n========== CURRENT PLAYER: ==========\n" << endl;

    cout << game.currentPlayer().getName() << endl;

    cout << "\n========== ROUND 1: ==========\n" << endl;

    cout << governor->getName() << " preforms gather" << endl;
    governor->gather(game);
    game.nextTurn();

    cout << spy->getName() << " preforms gather" << endl;
    spy->gather(game);
    game.nextTurn();

    cout << baron->getName() << " preforms gather" << endl;
    baron->gather(game);
    game.nextTurn();

    cout << general->getName() << " preforms gather" << endl;
    general->gather(game);
    game.nextTurn();

    cout << judge->getName() << " preforms gather" << endl;
    judge->gather(game);
    game.nextTurn();

    cout << "\n========== ROUND 2: ==========\n" << endl;

    cout << governor->getName() << " preforms gather" << endl;
    governor->gather(game);
    game.nextTurn();

    cout << spy->getName() << " preforms tax" << endl;
    spy->tax(game);
    game.nextTurn();

    cout << governor->getName() << " has " << governor->getCoins() << " coins." << endl;
    cout << spy->getName() << " has " << spy->getCoins() << " coins." << endl;

    cout << "\nCurrent turn: " << game.currentPlayer().getName() << endl;

    cout << baron->getName() << " preforms tax" << endl;
    baron->tax(game);
    game.nextTurn();

    cout << "\nCurrent turn: " << game.currentPlayer().getName() << endl;

    cout << general->getName() << " preforms gather" << endl;
    general->gather(game);
    game.nextTurn();

    cout << "\nCurrent turn: " << game.currentPlayer().getName() << endl;

    cout << judge->getName() << " preforms gather" << endl;
    judge->gather(game);
    game.nextTurn();

}
