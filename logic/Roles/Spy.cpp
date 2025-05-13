#include "Role.hpp"
#include "Game.hpp"
#include <iostream>

Spy::Spy(const std::string& name) : Player(name) {}

std::string Spy::getRole() const {
    return "Spy";
}

void Spy::arrest(Player& target, Game& game) {
    std::cout << getName() << " observes that " << target.getName() << " has " << target.getCoins() << " coins.\n";
    Player::arrest(target, game);
    // In full game logic, you might disable the target's arrest ability in next turn
}