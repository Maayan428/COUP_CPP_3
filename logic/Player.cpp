#include "Player.hpp"
#include "Game.hpp" //Interact with the Game object

Player::Player(const std::string& name) :
    name(name), coins(0), active(true) {}

std::string Player::getName() const {
    return name;
}

int Player::getCoins() const {
    return coins;
}

bool Player::isActive() const {
    return active;
}

void Player::addCoins(int amount) {
    if (amount < 0) {
        throw std::invalid_argument("Cannot add negative number of coins.");
    }
    coins += amount;
}

void Player::removeCoins(int amount) {
    if (amount < 0) {
        throw std::invalid_argument("Cannot remove negative number of coins.");
    }
    if (coins < amount) {
        throw std::runtime_error("Not enough coins to complete the action.");
    }
    coins -= amount;
}

// Marks the player as eliminated
void Player::eliminate() {
    active = false;
}

// Default implementation of gather (recieve 1 coin from the bank)
void Player::gather(Game& game) {
    if (game.isPlayerSanctioned(this)) throw std::runtime_error("You are sanctioned and cannot gather.");
    addCoins(1);
}

// Default implementation of tax (recieve 2 coins from the bank)
void Player::tax(Game& game) {
    game.attemptTax(this);
}

// Default implementation of bribe (pay 4 coins, get another turn)
void Player::bribe(Game& game) {
    game.attemptBribe(this);
}

// Default implementation of arrest (steal 1 coin from another player)
void Player::arrest(Player& target, Game& game) {
    if (!target.isActive()) {
        throw std::runtime_error("Cannot arrest an inactive player.");
    }
    if (target.getCoins() == 0) {
        throw std::runtime_error("Target player has no coins to steal.");
    }

    target.removeCoins(1);
    addCoins(1);
}

// Default implementation of sanction (pay 3 coins to restrict target's economy)
void Player::sanction(Player& target, Game& game) {
    removeCoins(3);
    game.applySanction(&target);
    if (target.getRole() == "Baron") {
        target.addCoins(1);
    }
}

// Default implementation of coup (pay 7 coins to eliminate another player)
void Player::coup(Player& target, Game& game) {
    if (!target.isActive()) {
        throw std::runtime_error("Target player is already eliminated.");
    }
    if (this == &target) {
        throw std::runtime_error("Cannot coup yourself.");
    }
    game.attemptCoup(this, target);
}

void Player::invest(Game&) {
    // Implemented in the Baron class
}