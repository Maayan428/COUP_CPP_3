#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <iostream>

Game::Game() :
    currentTurnIndex(0), extraTurnPlayer(nullptr) {}

void Game::addPlayer(Player* player) {
    if (players.size() >= 6) throw std::runtime_error("Too many players.");
    players.push_back(player);
}

Player& Game::currentPlayer() {
    if (players.empty()) throw std::runtime_error("No players.");
    int checked = 0;
    while (!players[currentTurnIndex]->isActive()) {
        currentTurnIndex = (currentTurnIndex + 1) % players.size();
        if (++checked > players.size()) throw std::runtime_error("No active players.");
    }
    return *players[currentTurnIndex];
}

void Game::nextTurn() {
    if (extraTurnPlayer != nullptr) {
        currentTurnIndex = getTurnIndexOf(extraTurnPlayer);
        extraTurnPlayer = nullptr;
        return;
    }

    int count = players.size();
    do {
        currentTurnIndex = (currentTurnIndex + 1) % count;
    } while (!players[currentTurnIndex]->isActive());
}

std::vector<std::string> Game::activePlayers() const {
    std::vector<std::string> result;
    for (auto p : players) {
        if (p->isActive()) result.push_back(p->getName());
    }
    return result;
}

std::string Game::winner() const {
    if (!isGameOver()) throw std::runtime_error("Game is not over.");
    for (auto p : players) {
        if (p->isActive()) return p->getName();
    }
    throw std::runtime_error("No active player found.");
}

bool Game::isGameOver() const {
    int count = 0;
    for (auto p : players) {
        if (p->isActive()) count++;
    }
    return count == 1;
}

void Game::setExtraTurn(Player* p) {
    extraTurnPlayer = p;
}

void Game::applySanction(Player* p) {
    sanctionedUntilNextTurn[p] = currentTurnIndex + 1; // Active until end of their next turn
}

bool Game::isPlayerSanctioned(Player* p) const {
    auto it = sanctionedUntilNextTurn.find(p);
    if (it == sanctionedUntilNextTurn.end()) return false;
    return currentTurnIndex <= it->second;
}

int Game::getTurnIndexOf(Player* p) const {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i] == p) return static_cast<int>(i);
    }
    throw std::runtime_error("Player not found in game.");
}

void Game::attemptBribe(Player* actor) {
    try {
        actor->removeCoins(4);
        for (Player* p : players) {
            if (p->isActive() && p != actor && p->canBlockBribe()) {
                std::cout << p->getName() << ", do you want to block the bribe of "
                          << actor->getName() << "? (y/n): ";
                char response;
                std::cin >> response;
                if (response == 'y') {
                    std::cout << "Bribe blocked by " << p->getName() << "\n";
                    return; // blocked, no extra turn
                }
            }
        }
        setExtraTurn(actor); // no one blocked, give extra turn
    } catch (const std::exception& e) {
        std::cerr << "Bribe failed: " << e.what() << "\n";
    }
}

void Game::attemptTax(Player* actor) {
    try {
        for (Player* p : players) {
            if (p->isActive() && p != actor && p->canBlockTax()) {
                std::cout << p->getName() << ", do you want to block the tax of "
                          << actor->getName() << "? (y/n): ";
                char response;
                std::cin >> response;
                if (response == 'y') {
                    std::cout << "Tax blocked by " << p->getName() << "\n";
                    return;
                }
            }
        }
        actor->addCoins(2); // no one blocked, pay 2
    } catch (const std::exception& e) {
        std::cerr << "Tax failed: " << e.what() << "\n";
    }
}

void Game::attemptCoup(Player* actor, Player& target) {
    try {
        for (Player* p : players) {
            if (p->isActive() && p != actor && p->canBlockCoup()) {
                std::cout << p->getName() << ", do you want to block the coup of "
                          << actor->getName() << " against " << target.getName() << "? (y/n): ";
                char response;
                std::cin >> response;
                if (response == 'y') {
                    std::cout << "Coup blocked by " << p->getName() << "\n";
                    actor->removeCoins(7); // still loses coins
                    return;
                }
            }
        }
        actor->removeCoins(7); // no one blocked, pay 7
        target.eliminate();
    } catch (const std::exception& e) {
        std::cerr << "Coup failed: " << e.what() << "\n";
    }
}
