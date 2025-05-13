#include "Role.hpp"
#include "Game.hpp"
#include <iostream>

Governor::Governor(const std::string& name) : Player(name) {}

std::string Governor::getRole() const {
    return "Governor";
}

void Governor::tax(Game& game) {
    if (game.isPlayerSanctioned(this)) {
        throw std::runtime_error("You are sanctioned and cannot use tax.");
    }
    addCoins(3); // Governor gets 3 instead of 2
}

bool Governor::canBlockTax() const {
    return true;
}
