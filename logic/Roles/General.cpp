//maayan428@gmail.com

#include "Role.hpp"
#include "Game.hpp"

General::General(const std::string& name) : Player(name) {}

std::string General::getRole() const {
    return "General";
}

bool General::canBlockCoup() const {
    return coins >= 5;
}
