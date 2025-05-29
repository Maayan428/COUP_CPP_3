//maayan428@gmail.com

#include "Role.hpp"
#include "Game.hpp"
#include <iostream>

Spy::Spy(const std::string& name) : Player(name) {}

std::string Spy::getRole() const {
    return "Spy";
}

// replace with the function - peak///////////////
