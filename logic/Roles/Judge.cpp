//maayan428@gmail.com

#include "Role.hpp"
#include "Game.hpp"

Judge::Judge(const std::string& name) : Player(name) {}

std::string Judge::getRole() const {
    return "Judge";
}

bool Judge::canBlockBribe() const {
    return true;
}