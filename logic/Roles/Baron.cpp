#include "Role.hpp"
#include "Game.hpp"

Baron::Baron(const std::string& name) : Player(name) {}

std::string Baron::getRole() const {
    return "Baron";
}

void Baron::gather(Game& game) {
    if (game.isPlayerSanctioned(this)) {
        throw std::runtime_error("You are sanctioned and cannot gather.");
    }
    // "Invest" 3 coins to gain 6
    if (coins >= 3) {
        removeCoins(3);
        addCoins(6);
    } else {
        // Fallback to regular gather if not enough coins to invest
        addCoins(1);
    }
}