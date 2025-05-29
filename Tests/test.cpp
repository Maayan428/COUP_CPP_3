//maayan428@gmail.com

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../headers/Game.hpp"
#include "../headers/Player.hpp"
#include "../headers/Role.hpp"
#include <stdexcept>

TEST_CASE("Game and Player basic flow") {
    Game game;
    Player* p1 = new Governor("Alice");
    Player* p2 = new Spy("Bob");

    CHECK_NOTHROW(game.addPlayer(p1));
    CHECK_NOTHROW(game.addPlayer(p2));

    CHECK(game.getPlayers().size() == 2);
    CHECK(game.activePlayers().size() == 2);

    p1->addCoins(3);
    CHECK(p1->getCoins() == 3);
    CHECK_NOTHROW(p1->removeCoins(2));
    CHECK(p1->getCoins() == 1);

    CHECK(p1->isActive());
    p1->eliminate();
    CHECK(!p1->isActive());

    delete p1;
    delete p2;
}

TEST_CASE("Turn advancement and sanctions") {
    Game game;
    Player* p1 = new Governor("Gil");
    Player* p2 = new Spy("Roi");

    game.addPlayer(p1);
    game.addPlayer(p2);

    CHECK(game.currentPlayer().getName() == "Gil");
    p1->gather(game);
    game.nextTurn();
    CHECK(game.currentPlayer().getName() == "Roi");

    CHECK_NOTHROW(game.applySanction(p1));
    CHECK(game.isPlayerSanctioned(p1));

    delete p1;
    delete p2;
}

TEST_CASE("Player actions: gather, tax, bribe, arrest, coup") {
    Game game;
    Player* p1 = new Baron("Amit");
    Player* p2 = new Merchant("Lior");

    game.addPlayer(p1);
    game.addPlayer(p2);

    p1->gather(game);
    CHECK(p1->getCoins() == 1);

    p1->tax(game);
    CHECK(p1->getCoins() == 3); // +2

    p1->addCoins(1); // total: 4
    CHECK_NOTHROW(p1->bribe(game)); // Pay 4, extra turn
    CHECK(p1->getCoins() == 0);

    p1->addCoins(7);
    CHECK_NOTHROW(p1->coup(*p2, game));
    CHECK(!p2->isActive());

    delete p1;
    delete p2;
}

TEST_CASE("Arrest rules and validation") {
    Game game;
    Player* p1 = new Spy("Amit");
    Player* p2 = new General("Noa");

    game.addPlayer(p1);
    game.addPlayer(p2);

    p2->addCoins(2);
    CHECK_NOTHROW(p1->arrest(*p2, game));
    CHECK(p1->getCoins() == 1); // General isn't affected, but Spy gets coin

    delete p1;
    delete p2;
}

