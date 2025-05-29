//maayan428@gmail.com

#include <SFML/Graphics.hpp>
#include "SFMLGui.hpp"
#include <iostream>
#include "Game.hpp" 
#include "Player.hpp"
#include <cmath>
#include "GUIErrorManager.hpp"


void launchGui() {

    Game game;
    sf::RenderWindow window(sf::VideoMode(800, 600), "COUP - SFML GUI");
    GameState state = GameState::Welcome;
    sf::Font font;

    if (!font.loadFromFile("GUI/arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text titleText, startText;
    sf::RectangleShape startButton;

    int playerCount = 2; // Defualt
    sf::Text playerCountText, selectText, continueText;
    sf::Text plusText, minusText;
    sf::RectangleShape plusButton, minusButton, continueButton;

    std::vector<std::string> playerNames;
    std::string currentNameInput;
    sf::Text nameInputText, enterNameTitle, addedPlayersListText;
    sf::RectangleShape addPlayerButton, startGameButton;
    sf::Text addPlayerButtonText, startGameButtonText;

    std::string lastActionTextStr = "";
    sf::Text lastActionText;
    
    GUIErrorManager errorManager;
    errorManager.init(font);

    int pendingAction = -1;
    std::vector<Player*> validTargets;
    int targetChoiceIndex = -1;
    bool showTargetPopup = false;
    bool awaitingTarget = false;
   
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == GameState::Welcome && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    state = GameState::SelectPlayerCount;
                }
            }

            if (state == GameState::SelectPlayerCount && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
                if (minusButton.getGlobalBounds().contains(mouse) && playerCount > 2)
                    playerCount--;
                if (plusButton.getGlobalBounds().contains(mouse) && playerCount < 6)
                    playerCount++;
                if (continueButton.getGlobalBounds().contains(mouse)) {
                    state = GameState::EnterNames;
                }
            }

            if (state == GameState::EnterNames && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
                if (addPlayerButton.getGlobalBounds().contains(mouse)) {
                    if (currentNameInput.empty()) {
                        errorManager.showError("Name cannot be empty.");
                    } else if (std::find(playerNames.begin(), playerNames.end(), currentNameInput) != playerNames.end()) {
                        errorManager.showError("Name already added.");
                    } else if (playerNames.size() >= static_cast<size_t>(playerCount)) {
                        errorManager.showError("Maximum number of players reached.");
                    } else {
                        playerNames.push_back(currentNameInput);
                        game.addPlayer(game.createRandomRole(currentNameInput));
                        currentNameInput.clear();
                        errorManager.clear();
                    }
                }
                if (startGameButton.getGlobalBounds().contains(mouse)) {
                    if (playerNames.size() != static_cast<size_t>(playerCount)) {
                        errorManager.showError("Please enter all player names before starting.");
                    } else {
                        state = GameState::ShowRoles;
                        errorManager.clear();
                    }
                }
            }

            if (state == GameState::EnterNames && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !currentNameInput.empty()) {
                    currentNameInput.pop_back();
                } else if (event.text.unicode < 128 && event.text.unicode != '\r') {
                    currentNameInput += static_cast<char>(event.text.unicode);
                }
            }

            if (state == GameState::ShowRoles && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
                if (continueButton.getGlobalBounds().contains(mouse)) {
                    state = GameState::InGame;
                }
            }

            if (state == GameState::InGame && !awaitingTarget && event.type ==
                    sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Num1: pendingAction = 1; break;
                    case sf::Keyboard::Num2: pendingAction = 2; break;
                    case sf::Keyboard::Num3: pendingAction = 3; break;
                    case sf::Keyboard::Num4: pendingAction = 4; break;
                    case sf::Keyboard::Num5: pendingAction = 5; break;
                    case sf::Keyboard::Num6: pendingAction = 6; break;
                    case sf::Keyboard::Num7: pendingAction = 7; break;
                    default: 
                        errorManager.showError("Invalid key. Please choose a valid action."); 
                        break;
                }

                Player& actor = game.currentPlayer();

                if (pendingAction == 1) {
                    game.currentPlayer().gather(game);
                    lastActionTextStr = game.currentPlayer().getName() + " performed Gather.";
                    advanceToNextValidPlayer(game);
                    pendingAction = -1;

                } else if (pendingAction == 2) {
                    if (guiAttemptTax(game, &game.currentPlayer(), window, font, errorManager)) {
                        lastActionTextStr = "Tax was blocked.";

                    } else {
                        lastActionTextStr = game.currentPlayer().getName() + " performed Tax.";
                        advanceToNextValidPlayer(game);
                    }
                    pendingAction = -1;

                } else if (pendingAction == 3) {
                    if (guiAttemptBribe(game, &game.currentPlayer(), window, font, errorManager)) {
                        lastActionTextStr = "Bribe was blocked.";
                    } else {
                        lastActionTextStr = game.currentPlayer().getName() + " performed Bribe.";
                        advanceToNextValidPlayer(game);
                    }
                    pendingAction = -1;

                } else if (pendingAction == 4 || pendingAction == 5 || pendingAction == 6) {
                    int cost = (pendingAction == 4 ? 3 : (pendingAction == 5 ? 2 : 7));
                    if (actor.getCoins() < cost) {
                        errorManager.showError("Not enough coins. You have $" + std::to_string(actor.getCoins()) + ", need $" + std::to_string(cost));
                        pendingAction = -1;
                    } else {
                        validTargets.clear();
                        for (Player* p : game.getPlayers()) {
                            if (p->isActive() && p != &actor) validTargets.push_back(p);
                        }
                        if (validTargets.empty()) {
                            errorManager.showError("No valid targets for this action.");
                            pendingAction = -1;
                        } else {
                            awaitingTarget = true;
                        }
                    }
                }
            }

            if (showTargetPopup && event.type == sf::Event::KeyPressed) {
                int index = event.key.code - sf::Keyboard::Num1;
                if (index >= 0 && index < static_cast<int>(validTargets.size())) {
                    Player* target = validTargets[index];
                    Player& actor = game.currentPlayer();
                    bool blocked = false;

                    try {
                        switch (pendingAction) {
                            case 4:
                                if (game.getLastArrested() == target) {
                                    errorManager.showError("You cannot arrest the same player twice in a row.");
                                    break;      
                                }
                                blocked = guiAttemptArrest(game, &actor, target, window, font, errorManager);
                                if (!blocked) {
                                    game.setLastArrested(target);
                                    lastActionTextStr = actor.getName() + " arrested " + target->getName();
                                } else {
                                    lastActionTextStr = "Arrest was blocked.";
                                }
                                break;
                                
                            case 5:
                                blocked = guiAttemptSanction(game, &actor, target, window, font, errorManager);
                                if (!blocked)
                                    lastActionTextStr = actor.getName() + " sanctioned " + target->getName();
                                else
                                    lastActionTextStr = "Sanction was blocked.";
                                break;
                            case 6:
                                blocked = guiAttemptCoup(game, &actor, target, window, font, errorManager);
                                if (!blocked)
                                    lastActionTextStr = actor.getName() + " performed Coup on " + target->getName();
                                else
                                    lastActionTextStr = "Coup was blocked.";
                                break;
                        }
                        advanceToNextValidPlayer(game);

                    } catch (const std::exception& e) {
                        errorManager.showError(e.what());
                        lastActionTextStr = std::string("Error: ") + e.what();
                    }
                    pendingAction = -1;
                    awaitingTarget = false;
                } else {
                    errorManager.showError("Invalid target number.");
                }
                showTargetPopup = false;
            }
        }

        window.clear(sf::Color::White);

        if (state == GameState::Welcome)
            drawWelcomeScreen(window, font, titleText, startButton, startText);
        else if (state == GameState::SelectPlayerCount)
            drawSelectPlayerCountScreen(window, font, selectText, playerCountText, minusButton, plusButton, continueButton, continueText, minusText, plusText, playerCount);
        else if (state == GameState::EnterNames)
            drawEnterNamesScreen(window, font, enterNameTitle, nameInputText, addPlayerButton, addPlayerButtonText, startGameButton, startGameButtonText, addedPlayersListText, playerNames, currentNameInput, playerCount);
        else if (state == GameState::ShowRoles)
            drawShowRolesScreen(window, font, game.getPlayers(), continueButton, continueText);
        else if (state == GameState::InGame) {
            drawInGameScreen(window, font, game, lastActionText, lastActionTextStr);
            if (awaitingTarget){
                drawTargetPopup(window, font, validTargets);
                showTargetPopup = true;
            }
        }

        errorManager.draw(window);
        window.display();
    }
}

void drawWelcomeScreen(sf::RenderWindow& window, sf::Font& font, sf::Text& titleText, sf::RectangleShape& startButton, sf::Text& startText) {
    window.clear(sf::Color(240, 248, 255)); // light blueish background (AliceBlue)

    // Title text
    titleText.setFont(font);
    titleText.setString("Welcome to COUP");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color(20, 60, 100)); // navy-like color
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition(140, 80);

    // Button
    startButton.setSize(sf::Vector2f(220, 70));
    startButton.setFillColor(sf::Color(100, 180, 255));  // light blue
    startButton.setOutlineThickness(3);
    startButton.setOutlineColor(sf::Color(30, 90, 160)); // darker blue
    startButton.setPosition(290, 300);

    // Text in button
    startText.setFont(font);
    startText.setString("Start Game");
    startText.setCharacterSize(28);
    startText.setFillColor(sf::Color::White);
    startText.setStyle(sf::Text::Bold);
    startText.setPosition(
    startButton.getPosition().x + 35,
    startButton.getPosition().y + 18
    );

    window.draw(titleText);
    window.draw(startButton);
    window.draw(startText);
}

void drawSelectPlayerCountScreen(sf::RenderWindow& window, sf::Font& font,
    sf::Text& selectText, sf::Text& playerCountText,
    sf::RectangleShape& minusButton, sf::RectangleShape& plusButton,
    sf::RectangleShape& continueButton, sf::Text& continueText,
    sf::Text& minusText, sf::Text& plusText,
    int playerCount) {

    // Title text
    selectText.setFont(font);
    selectText.setString("Select Number of Players");
    selectText.setCharacterSize(40);
    selectText.setFillColor(sf::Color(20, 60, 100));
    selectText.setStyle(sf::Text::Bold);
    selectText.setPosition(130, 80);

    // Title num of players
    playerCountText.setFont(font);
    playerCountText.setString(std::to_string(playerCount));
    playerCountText.setCharacterSize(48);
    playerCountText.setFillColor(sf::Color(30, 90, 160));
    playerCountText.setPosition(375, 200);

    // Button -
    minusButton.setSize(sf::Vector2f(50, 50));
    minusButton.setFillColor(sf::Color(100, 180, 255));
    minusButton.setOutlineThickness(2);
    minusButton.setOutlineColor(sf::Color(30, 90, 160));
    minusButton.setPosition(300, 200);

    // Button +
    plusButton.setSize(sf::Vector2f(50, 50));
    plusButton.setFillColor(sf::Color(100, 180, 255));
    plusButton.setOutlineThickness(2);
    plusButton.setOutlineColor(sf::Color(30, 90, 160));
    plusButton.setPosition(460, 200);

    // Places the number exactly in the middle between + and -.
    float left = minusButton.getPosition().x;
    float right = plusButton.getPosition().x + plusButton.getSize().x;
    float centerX = (left + right) / 2.0f;

    sf::FloatRect bounds = playerCountText.getLocalBounds();
    playerCountText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    playerCountText.setPosition(centerX, 225); 
    
    // Button Continue
    continueButton.setSize(sf::Vector2f(180, 50));
    continueButton.setFillColor(sf::Color(100, 180, 255));
    continueButton.setOutlineThickness(2);
    continueButton.setOutlineColor(sf::Color(30, 90, 160));
    continueButton.setPosition(310, 350);

    continueText.setFont(font);
    continueText.setString("Continue");
    continueText.setCharacterSize(24);
    continueText.setFillColor(sf::Color::White);
    continueText.setStyle(sf::Text::Bold);
    continueText.setPosition(345, 360);

    plusText.setFont(font);
    plusText.setString("+");
    plusText.setCharacterSize(32);
    plusText.setFillColor(sf::Color(20, 60, 100)); 
    plusText.setStyle(sf::Text::Bold);
    plusText.setPosition(
    plusButton.getPosition().x + 13,
    plusButton.getPosition().y + 5
    );

    minusText.setFont(font);
    minusText.setString("-");
    minusText.setCharacterSize(32);
    minusText.setFillColor(sf::Color(20, 60, 100));
    minusText.setStyle(sf::Text::Bold);
    minusText.setPosition(
    minusButton.getPosition().x + 16,
    minusButton.getPosition().y + 5
);


    // Draw
    window.draw(selectText);
    window.draw(playerCountText);
    window.draw(minusButton);
    window.draw(plusButton);
    window.draw(minusText);
    window.draw(plusText);
    window.draw(continueButton);
    window.draw(continueText);
}

void drawEnterNamesScreen(sf::RenderWindow& window, sf::Font& font,
    sf::Text& enterNameTitle, sf::Text& nameInputText,
    sf::RectangleShape& addPlayerButton, sf::Text& addPlayerButtonText,
    sf::RectangleShape& startGameButton, sf::Text& startGameButtonText,
    sf::Text& addedPlayersListText,
    const std::vector<std::string>& playerNames,
    const std::string& currentNameInput,
    int playerCount) {

// Title
enterNameTitle.setFont(font);
enterNameTitle.setString("Enter player names:");
enterNameTitle.setCharacterSize(32);
enterNameTitle.setFillColor(sf::Color(20, 60, 100));
enterNameTitle.setPosition(220, 50);

// Input text
nameInputText.setFont(font);
nameInputText.setString(currentNameInput + "|");
nameInputText.setCharacterSize(28);
nameInputText.setFillColor(sf::Color::Black);
nameInputText.setPosition(200, 120);

// Add Player button
addPlayerButton.setSize(sf::Vector2f(140, 40));
addPlayerButton.setFillColor(sf::Color(100, 180, 255));
addPlayerButton.setOutlineThickness(2);
addPlayerButton.setOutlineColor(sf::Color(30, 90, 160));
addPlayerButton.setPosition(370, 180);

addPlayerButtonText.setFont(font);
addPlayerButtonText.setString("Add Player");
addPlayerButtonText.setCharacterSize(20);
addPlayerButtonText.setFillColor(sf::Color::White);
addPlayerButtonText.setPosition(385, 187);

// Start Game button
startGameButton.setSize(sf::Vector2f(160, 45));
startGameButton.setFillColor(sf::Color(60, 130, 200));
startGameButton.setOutlineThickness(2);
startGameButton.setOutlineColor(sf::Color(30, 90, 160));
startGameButton.setPosition(320, 500);

startGameButtonText.setFont(font);
startGameButtonText.setString("Start Game");
startGameButtonText.setCharacterSize(22);
startGameButtonText.setFillColor(sf::Color::White);
startGameButtonText.setPosition(345, 510);

// List of added players
std::string list = "Players:\n";
for (size_t i = 0; i < playerNames.size(); ++i) {
list += std::to_string(i + 1) + ". " + playerNames[i] + "\n";
}
addedPlayersListText.setFont(font);
addedPlayersListText.setString(list);
addedPlayersListText.setCharacterSize(20);
addedPlayersListText.setFillColor(sf::Color::Black);
addedPlayersListText.setPosition(200, 250);

// Draw all
window.draw(enterNameTitle);
window.draw(nameInputText);
window.draw(addPlayerButton);
window.draw(addPlayerButtonText);
window.draw(startGameButton);
window.draw(startGameButtonText);
window.draw(addedPlayersListText);
}

void drawShowRolesScreen(sf::RenderWindow& window, sf::Font& font,
    const std::vector<Player*>& players,
    sf::RectangleShape& continueButton, sf::Text& continueText) {
window.clear(sf::Color(240, 248, 255));  

float y = 100;
for (Player* p : players) {
sf::Text nameText;
nameText.setFont(font);
nameText.setString(p->getName() + " - Role: " + p->getRole());
nameText.setCharacterSize(28);
nameText.setFillColor(sf::Color::Black);
nameText.setPosition(100, y);
y += 50;

window.draw(nameText);
}

// Continue button
continueButton.setSize(sf::Vector2f(200, 50));
continueButton.setFillColor(sf::Color(70, 130, 180)); 
continueButton.setPosition(300, y + 40);

continueText.setFont(font);
continueText.setString("Continue");
continueText.setCharacterSize(24);
continueText.setFillColor(sf::Color::White);
continueText.setPosition(340, y + 50);

window.draw(continueButton);
window.draw(continueText);
}

void drawInGameScreen(sf::RenderWindow& window, sf::Font& font, Game& game, sf::Text& lastActionText, const std::string& lastActionTextStr) {
    window.clear(sf::Color(240, 248, 255));

    // Circle table in the middle
    float centerX = 400, centerY = 300;
    float tableRadius = 150;
    sf::CircleShape table(tableRadius);
    table.setFillColor(sf::Color(200, 220, 255));
    table.setOrigin(tableRadius, tableRadius);
    table.setPosition(centerX, centerY);
    window.draw(table);

    // Information about players
    const std::vector<Player*>& players = game.getPlayers();
    int numPlayers = players.size();
    float angleStep = 2 * M_PI / numPlayers;
    float orbitRadius = 220;
    float playerRadius = 30;

    for (int i = 0; i < numPlayers; ++i) {
        Player* p = players[i];
        float angle = i * angleStep - M_PI / 2;
        float x = centerX + orbitRadius * cos(angle);
        float y = centerY + orbitRadius * sin(angle);

        sf::CircleShape playerCircle(playerRadius);
        playerCircle.setOrigin(playerRadius, playerRadius);
        playerCircle.setPosition(x, y);

        if (!p->isActive()) {
            playerCircle.setFillColor(sf::Color(180, 50, 50));  // Couped - red
        } else if (game.isPlayerSanctioned(p)) {
            playerCircle.setFillColor(sf::Color(160, 160, 160));  // Sanctioned - gray
        } else if (&game.currentPlayer() == p) {
            playerCircle.setFillColor(sf::Color(50, 200, 50));  // Current - green
        } else {
            playerCircle.setFillColor(sf::Color(120, 180, 255));  // Player - blue
        }

        playerCircle.setOutlineThickness(2);
        playerCircle.setOutlineColor(sf::Color::Black);
        window.draw(playerCircle);

        // Display name & role
        sf::Text label;
        label.setFont(font);
        label.setCharacterSize(16);
        label.setFillColor(sf::Color::Black);
        label.setString(p->getName() + "\n(" + p->getRole() + ")\n$" + std::to_string(p->getCoins()));
        label.setPosition(x - playerRadius, y + playerRadius + 5);
        window.draw(label);
    }

    // Actions menu
    sf::Text actionsText;
    actionsText.setFont(font);
    actionsText.setCharacterSize(20);
    actionsText.setFillColor(sf::Color::Black);
    std::string text = "Available actions:\n1. Gather\n2. Tax\n3. Bribe\n4. Arrest\n5. Sanction\n6. Coup";
    if (game.currentPlayer().getRole() == "Baron") text += "\n7. Invest";
    if (game.currentPlayer().getRole() == "Spy") text += "\n7. Peak";
    actionsText.setString(text);
    actionsText.setPosition(650, 100);
    window.draw(actionsText);

    // Recent action text
    lastActionText.setFont(font);
    lastActionText.setCharacterSize(18);
    lastActionText.setFillColor(sf::Color::Black);
    lastActionText.setPosition(20, 550);
    lastActionText.setString("Last Action: " + lastActionTextStr);
    window.draw(lastActionText);
}

void drawTargetPopup(sf::RenderWindow& window, sf::Font& font, const std::vector<Player*>& targets) {
    sf::RectangleShape box(sf::Vector2f(400, 300));
    box.setFillColor(sf::Color(255, 255, 255, 245));
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(2);
    box.setPosition(200, 150);
    window.draw(box);

    sf::Text title("Choose Target:", font, 24);
    title.setFillColor(sf::Color::Black);
    title.setPosition(220, 160);
    window.draw(title);

    float y = 200;
    for (size_t i = 0; i < targets.size(); ++i) {
        Player* p = targets[i];
        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(20);
        line.setFillColor(sf::Color(20, 60, 100));
        line.setString(std::to_string(i + 1) + ". " + p->getName() + " (" + p->getRole() + ") $" + std::to_string(p->getCoins()));
        line.setPosition(220, y);
        y += 30;
        window.draw(line);
    }
}

void advanceToNextValidPlayer(Game& game) {
    game.nextTurn();  
    while (!game.currentPlayer().isActive() || game.isPlayerSanctioned(&game.currentPlayer())) {
        game.nextTurn();
    }
}

void drawBlockPrompt(sf::RenderWindow& window, sf::Font& font, const std::string& message) {
    window.clear(sf::Color::White);  // clear screen

    // Box
    sf::RectangleShape box(sf::Vector2f(400, 200));
    box.setFillColor(sf::Color(255, 255, 255, 245));
    box.setOutlineColor(sf::Color::Black);
    box.setOutlineThickness(2);
    box.setPosition(200, 200);
    window.draw(box);

    // Main message (question)
    sf::Text questionText(message, font, 24);
    questionText.setFillColor(sf::Color::Black);
    sf::FloatRect qBounds = questionText.getLocalBounds();
    questionText.setOrigin(qBounds.width / 2, qBounds.height / 2);
    questionText.setPosition(box.getPosition().x + box.getSize().x / 2, box.getPosition().y + 60);
    window.draw(questionText);

    // Instructions (Y/N)
    sf::Text instructions("Press Y to block or N to allow.", font, 20);
    instructions.setFillColor(sf::Color(80, 80, 80));
    sf::FloatRect iBounds = instructions.getLocalBounds();
    instructions.setOrigin(iBounds.width / 2, iBounds.height / 2);
    instructions.setPosition(box.getPosition().x + box.getSize().x / 2, box.getPosition().y + 120);
    window.draw(instructions);

    window.display();
}


// Waits for y/n response from the keyboard
bool waitForBlockResponse(sf::RenderWindow& window,
    GUIErrorManager& errorManager) {

    sf::Clock errorClock;
    bool showError = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key key = event.key.code;

                if (key == sf::Keyboard::Y || key == sf::Keyboard::N) {
                    return key == sf::Keyboard::Y;
                } else {
                    errorManager.showError("Invalid key. Please press Y or N.");
                    showError = true;
                    errorClock.restart();                }
            }
        }
        showError = true;
        errorClock.restart();
    }
    if (showError && errorClock.getElapsedTime().asSeconds() > 2.0f) {
        errorManager.clear();
        showError = false;
    }
    return false;
}

bool askBlockers(Game& game,
                 Player* actor,
                 const std::string& action,
                 const std::vector<Player*>& blockers,
                 sf::RenderWindow& window,
                 sf::Font& font,
                 GUIErrorManager& errorManager) {

    sf::Clock errorClock;
    bool showError = false;

    std::string actionLower = action;
    std::transform(actionLower.begin(), actionLower.end(), actionLower.begin(), ::tolower);

    for (Player* p : blockers) {
        if (!p->isActive() || p == actor) continue;

        // Casse 1: General blocking a coup:
        if (actionLower == "coup" && p->getRole() == "General") {
            if (p->getCoins() < 5) {
                errorManager.showError(p->getName() + " doesn't have enough coins (5) to block a coup.");
                showError = true;
                errorClock.restart();
                continue;
            }
    
            std::string prompt = p->getName() + ", do you want to block the coup of " + actor->getName() + "? (Y/N)";
            drawBlockPrompt(window, font, prompt);
    
            if (waitForBlockResponse(window, errorManager)) {
                try {
                    if(p->getRole() == "General"){
                        p->removeCoins(5);
                    }
                    errorManager.showError("Coup was blocked by " + p->getName() + " ("+ p->getRole() +")");
                    showError = true;
                    errorClock.restart();
                    return true;
                } catch (const std::exception& e) {
                    errorManager.showError("Failed to block: " + std::string(e.what()));
                    showError = true;
                    errorClock.restart();
                    continue;
                }
            }
            continue; // Didnt block - move on to next player
        }

        // Casse 2: Spy blocking an arrest:
        if (actionLower == "arrest" && p->getRole() == "Spy") {
            std::string prompt = p->getName() + ", do you want to block the arrest of " + actor->getName() + "? (Y/N)";
            drawBlockPrompt(window, font, prompt);

            if (waitForBlockResponse(window, errorManager)) {
                try {
                    errorManager.showError("Arrest was blocked by " + p->getName() + " (Spy)");
                    showError = true;
                    errorClock.restart();                    
                    return true;
                } catch (const std::exception& e) {
                    errorManager.showError("Failed to block: " + std::string(e.what()));
                    showError = true;
                    errorClock.restart();                    
                    continue;
                }
            }
        }

        // Casse 3: Judge blocking a bribe:
        if (actionLower == "bribe" && p->getRole() == "Judge") {
            std::string prompt = p->getName() + ", do you want to block the bribe of " + actor->getName() + "? (Y/N)";
            drawBlockPrompt(window, font, prompt);

            if (waitForBlockResponse(window, errorManager)) {
                try {
                    errorManager.showError("Bribe was blocked by " + p->getName() + " (Judge)");
                    showError = true;
                    errorClock.restart();                    
                    return true;
                } catch (const std::exception& e) {
                    errorManager.showError("Failed to block: " + std::string(e.what()));
                    showError = true;
                    errorClock.restart();                    
                    continue;
                }
            }
        }

        // Casse 4: Governor blocking a tax:
        if (actionLower == "tax" && p->getRole() == "Governor") {
            std::string prompt = p->getName() + ", do you want to block the tax of " + actor->getName() + "? (Y/N)";
            drawBlockPrompt(window, font, prompt);

            if (waitForBlockResponse(window, errorManager)) {
                try {
                    errorManager.showError("Tax was blocked by " + p->getName() + " (Governor)");
                    showError = true;
                    errorClock.restart();                    
                    return true;
                } catch (const std::exception& e) {
                    errorManager.showError("Failed to block: " + std::string(e.what()));
                    showError = true;
                    errorClock.restart();                    
                    continue;
                }
            }
        }

        if (showError && errorClock.getElapsedTime().asSeconds() > 2.0f) {
            errorManager.clear();
            showError = false;
        }
    }
    return false;
}

bool guiAttemptTax(Game& game, Player* actor, sf::RenderWindow& window, sf::Font& font, GUIErrorManager& errorManager) {
    const auto& players = game.getPlayers();
    if (askBlockers(game, actor, "Tax", players, window, font, errorManager)) {
        return true; // blocked
    }

    try {
        if (actor->getRole() == "Governor") {
            actor->addCoins(3);
        } else {
            actor->addCoins(2);
        }
        return false;
    } catch (const std::exception& e) {
        errorManager.showError("Tax failed: " + std::string(e.what()));
        return true;
    }
}

bool guiAttemptBribe(Game& game, Player* actor, sf::RenderWindow& window, sf::Font& font, GUIErrorManager& errorManager) {
    if (actor->getCoins() < 4) {
        errorManager.showError("Not enough coins to perform bribe (need 4).");
        return true;
    }

    actor->removeCoins(4);
    const auto& players = game.getPlayers();

    if (askBlockers(game, actor, "Bribe", players, window, font, errorManager)) {
        return true;
    }

    game.setExtraTurn(actor);
    return false;
}

bool guiAttemptCoup(Game& game, Player* actor, Player* target, sf::RenderWindow& window, sf::Font& font, GUIErrorManager& errorManager) {
    if (actor->getCoins() < 7) {
        errorManager.showError("Not enough coins to perform coup (need 7).");
        return true;
    }

    const auto& players = game.getPlayers();

    if (askBlockers(game, actor, "Coup", players, window, font, errorManager)) {
        actor->removeCoins(7); 
        return true;
    }

    actor->removeCoins(7);
    target->eliminate();
    return false;
}

bool guiAttemptArrest(Game& game, Player* actor, Player* target, sf::RenderWindow& window, sf::Font& font, GUIErrorManager& errorManager) {
    if (askBlockers(game, actor, "Arrest", game.getPlayers(), window, font, errorManager)) {
        return true; 
    }

    try {
        actor->arrest(*target, game);
        return false;
    } catch (const std::exception& e) {
        errorManager.showError("Arrest failed: " + std::string(e.what()));
        return true;
    }
}

bool guiAttemptSanction(Game& game, Player* actor, Player* target, sf::RenderWindow& window, sf::Font& font, GUIErrorManager& errorManager) {
    if (actor->getCoins() < 3) {
        errorManager.showError("Not enough coins to perform sanction (need 3).");
        return true;
    }

    if (askBlockers(game, actor, "Sanction", game.getPlayers(), window, font, errorManager)) {
        return true; // Action blocked
    }

    try {
        actor->sanction(*target, game);
        return false;
    } catch (const std::exception& e) {
        errorManager.showError("Sanction failed: " + std::string(e.what()));
        return true;
    }
}