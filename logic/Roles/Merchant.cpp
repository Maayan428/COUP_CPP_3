#include "Role.hpp"
#include "Game.hpp"

Merchant::Merchant(const std::string& name) : Player(name) {}

std::string Merchant::getRole() const {
    return "Merchant";
}

void Merchant::gather(Game& game) {
    if (game.isPlayerSanctioned(this)) {
        throw std::runtime_error("You are sanctioned and cannot gather.");
    }
    if (getCoins() >= 3) {
        addCoins(2); // 1 for gather + 1 bonus
    } else {
        addCoins(1);
    }
}
