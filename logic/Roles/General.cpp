#include "Role.hpp"
#include "Game.hpp"

General::General(const std::string& name) : Player(name) {}

std::string General::getRole() const {
    return "General";
}

bool General::canBlockCoup() const {
    return coins >= 5;
}

void General::arrest(Player& target, Game& game) {
    Player::arrest(target, game);
    target.addCoins(1); // Refund to the General if he was the victim
}