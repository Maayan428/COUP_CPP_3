#include "Game.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <iostream>
#include <random>
#include "Role.hpp"

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

// Moves the turn to the next player
void Game::nextTurn() {
    // If a player has an extra turn
    if (extraTurnPlayer != nullptr) {
        currentTurnIndex = getTurnIndexOf(extraTurnPlayer);
        extraTurnPlayer = nullptr;
        return;
    }

    size_t count = players.size();
    size_t tries = 0;
    
    do {
        currentTurnIndex = (currentTurnIndex + 1) % count;
        ++tries;
    } while ((!players[currentTurnIndex]->isActive() || isPlayerSanctioned(players[currentTurnIndex])) && tries <= count);
    if (tries > count){
        throw std::runtime_error("No active players to advance to.");
    }  
    Player* p = players[currentTurnIndex];
    if (sanctionedUntilNextTurn.count(p) > 0 && sanctionedUntilNextTurn[p] <= currentTurnIndex) {
        sanctionedUntilNextTurn.erase(p); // Remove sanction
    }  
}

const std::vector<Player*>& Game::getPlayers() const {
    return players;
}

std::vector<std::string> Game::activePlayers() const {
    std::vector<std::string> result;  // Sets a new vector to show the result in
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

bool Game::hasValidTargets(Player* current) const {
    for (Player* p : players) {
        if (p->isActive() && p != current) return true;
    }
    return false;
}

void Game::setExtraTurn(Player* p) {
    extraTurnPlayer = p;
}

void Game::applySanction(Player* p) {
    sanctionedUntilNextTurn[p] = currentTurnIndex + 1; // Active until end of their next turn
}

bool Game::isPlayerSanctioned(Player* p) const {
    return sanctionedUntilNextTurn.count(p);

    //auto it = sanctionedUntilNextTurn.find(p);
    //if (it == sanctionedUntilNextTurn.end()) return false;
    //return currentTurnIndex <= it->second;
}

int Game::getTurnIndexOf(Player* p) const {
    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i] == p) return static_cast<int>(i);
    }
    throw std::runtime_error("Player not found in game.");
}

void Game::run() {
    std::cout << "Welcome to COUP!\n";
    initializePlayers();
    mainGameLoop();
    std::cout << "\n🏆 Winner is: " << winner() << " 🏆\n";
}

// this function is not called in the GUI version of the game.
void Game::initializePlayers() {
    int numPlayers;
    std::cout << "Enter number of players (2-6): ";
    std::cin >> numPlayers;
    while (numPlayers < 2 || numPlayers > 6) {
        std::cout << "Invalid number. Try again: ";
        std::cin >> numPlayers;
    }
    for (int i = 0; i < numPlayers; ++i) {
        std::string name;
        std::cout << "Enter name for player " << i + 1 << ": ";
        std::cin >> name;
        Player* player = createRandomRole(name);
        std::cout << name << " received role: " << player->getRole() << "\n";
        addPlayer(player);
    }
}

Player* Game::createRandomRole(const std::string& name) {
    static const std::vector<std::string> roles = {
        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
    };

    // Chooses a random index in the vector of roles
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, roles.size() - 1);

    std::string chosen = roles[dist(gen)];

    if (chosen == "Governor") return new Governor(name);
    if (chosen == "Spy")      return new Spy(name);
    if (chosen == "Baron")    return new Baron(name);
    if (chosen == "General")  return new General(name);
    if (chosen == "Judge")    return new Judge(name);
    if (chosen == "Merchant") return new Merchant(name);

    throw std::runtime_error("Unknown role: " + chosen);
}

void Game::mainGameLoop() {
    while (!isGameOver()) {
        Player& current = currentPlayer();

        std::cout << "\n--- Turn: " << current.getName()
                  << " (" << current.getRole() << ") ---\n";
        std::cout << "Coins: " << current.getCoins() << "\n";

        // OPTION - COUP 10+
        if (current.getCoins() >= 10) {
            std::cout << "You have 10+ coins, you must perform a coup!\n";
            Player* target = selectTarget(&current);
            current.coup(*target, *this);
            nextTurn();
            continue;
        }

        // Player chooses next move
        displayActions(current);
        int choice;
        std::cin >> choice;
        handleAction(choice, current);

        // Move turn to the next player
        if (!extraTurnPlayer) nextTurn();
        else extraTurnPlayer = nullptr;
    }
}

void Game::displayActions(const Player& player) {
    std::cout << "Available actions:\n";
    std::cout << "1. Gather\n 2. Tax\n 3. Bribe\n 4. Arrest\n 5. Sanction\n 6. Coup\n";
    if (player.getRole() == "Baron") std::cout << "7. Invest\n";
    std::cout << "Enter action number: ";
}

void Game::handleAction(int choice, Player& player) {
    try {
        switch (choice) {
            case 1: player.gather(*this);
                break;

            case 2: player.tax(*this);
                break;

            case 3: player.bribe(*this);
                break;

            case 4:
                if (hasValidTargets(&player)) {
                    player.arrest(*selectTarget(&player), *this);
                } else {
                    throw std::runtime_error("No valid players to arrest.");
                }
                break;

            case 5:
                if (hasValidTargets(&player)) {
                    player.sanction(*selectTarget(&player), *this);
                } else {
                    throw std::runtime_error("No valid players to sanction.");
                }
                break;

            case 6:
                if (player.getCoins() < 7)
                    throw std::runtime_error("Not enough coins to perform coup (need 7).");

                if (hasValidTargets(&player)) {
                    player.coup(*selectTarget(&player), *this);
                } else {
                    throw std::runtime_error("No valid players to coup.");
                }
                break;

            case 7:
                if (player.getRole() == "Baron") {
                    player.invest(*this);
                } else {
                    throw std::runtime_error("Only the Baron can perform invest.");
                }
                break;

            default:
                throw std::runtime_error("Invalid action selected.");
        }
    } catch (const std::exception& e) {
        throw;
    }
}

// Displays all active players to choose from - who to coup
Player* Game::selectTarget(Player* current) {
    std::vector<Player*> targets;
    for (Player* p : players) {
        if (p->isActive() && p != current) targets.push_back(p);
    }
    std::cout << "Select target:\n";
    for (size_t i = 0; i < targets.size(); ++i) {
        std::cout << i + 1 << ". " << targets[i]->getName() << "\n";
    }
    int choice;
    std::cin >> choice;
    while (choice < 1 || choice > static_cast<int>(targets.size())) {
        std::cout << "Invalid choice. Try again: ";
        std::cin >> choice;
    }
    return targets[choice - 1];
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
        // no one blocked, pay 2 / 3
        if (actor->getRole() == "Governor") {
            actor->addCoins(3);
        } else {
            actor->addCoins(2);
        }
        
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
