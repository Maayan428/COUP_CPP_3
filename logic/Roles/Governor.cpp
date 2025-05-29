//maayan428@gmail.com

#include "Role.hpp"
#include "Game.hpp"
#include <iostream>

Governor::Governor(const std::string& name) : Player(name) {}

std::string Governor::getRole() const {
    return "Governor";
}

bool Governor::canBlockTax() const {
    return true;
}
