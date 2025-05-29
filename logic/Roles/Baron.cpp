//maayan428@gmail.com

#include "Role.hpp"
#include "Game.hpp"

Baron::Baron(const std::string& name) : Player(name) {}

std::string Baron::getRole() const {
    return "Baron";
}

void Baron:: invest(Game& game) {
    if (coins >= 3) {
        removeCoins(3);
        addCoins(6);
    } else {
        throw std::runtime_error("Not enough coins to invest (requires 3).");
    }
}