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
   
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Welcome page
            if (state == GameState::Welcome && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    state = GameState::SelectPlayerCount;
                    std::cout << "Start Game clicked → going to SelectPlayerCount\n";
                }
            }

            // Select num of players page
            if (state == GameState::SelectPlayerCount && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
            
                if (minusButton.getGlobalBounds().contains(mouse) && playerCount > 2)
                    playerCount--;
            
                if (plusButton.getGlobalBounds().contains(mouse) && playerCount < 6)
                    playerCount++;
            
                if (continueButton.getGlobalBounds().contains(mouse)) {
                    std::cout << "Players selected: " << playerCount << "\n";
                    state = GameState::EnterNames;
                }
            }
    
            // Choose names page
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
                        errorManager.clear(); // clear any previous error
                    }
                }
        
                if (startGameButton.getGlobalBounds().contains(mouse)){
                    if (playerNames.size() != static_cast<size_t>(playerCount)) {
                        errorManager.showError("Please enter all player names before starting.");
                    } else {
                        state = GameState::ShowRoles;
                        std::cout << "All players added. Starting game...\n";
                        errorManager.clear();
                    }
                }
            }
    
            // Keyboard input - name
            if (state == GameState::EnterNames && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !currentNameInput.empty()) {
                    currentNameInput.pop_back();  
                } else if (event.text.unicode < 128 && event.text.unicode != '\r') {
                    currentNameInput += static_cast<char>(event.text.unicode);
                }
            }
        
            // Displays all players and roles
            if (state == GameState::ShowRoles && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
                if (continueButton.getGlobalBounds().contains(mouse)) {
                    state = GameState::InGame;
                    std::cout << "Game started!\n";
                }
            }

            // Main game board
            if (state == GameState::InGame && event.type == sf::Event::KeyPressed) {
                int action = -1;
                switch (event.key.code) {
                    case sf::Keyboard::Num1: action = 1; break;
                    case sf::Keyboard::Num2: action = 2; break;
                    case sf::Keyboard::Num3: action = 3; break;
                    case sf::Keyboard::Num4: action = 4; break;
                    case sf::Keyboard::Num5: action = 5; break;
                    case sf::Keyboard::Num6: action = 6; break;
                    case sf::Keyboard::Num7: action = 7; break;
                }
            
                if (action != -1 && !game.isGameOver()) {
                    Player& player = game.currentPlayer();
                    if (action == 4 || action == 5 || action == 6) {
                        int cost = 0;
                        switch (action) {
                            case 4: cost = 3; break; // Arrest
                            case 5: cost = 2; break; // Sanction
                            case 6: cost = 7; break; // Coup
                        }
                    
                        if (player.getCoins() < cost) {
                            errorManager.showError("Not enough coins for this action. You have $" + std::to_string(player.getCoins()) + ", need $" + std::to_string(cost) + ".");
                            continue;
                        }
                    }

                    if (action == 4 || action == 5 || action == 6) {
                        int cost = (action == 4 ? 3 : (action == 5 ? 2 : 7));
                        if (player.getCoins() < cost) {
                            errorManager.showError("Not enough coins for this action. You have $" + std::to_string(player.getCoins()) + ", need $" + std::to_string(cost) + ".");
                            continue;
                        }
                        validTargets.clear();
                        for (Player* p : game.getPlayers()) {
                            if (p->isActive() && p != &player) validTargets.push_back(p);
                        }
                        if (validTargets.empty()) {
                            errorManager.showError("No valid targets for this action.");
                            continue;
                        }
                        pendingAction = action;
                        showTargetPopup = true;
                        continue;
                    }
                    
                    try {
                        game.handleAction(action, player);
                        lastActionTextStr = player.getName() + " performed action " + std::to_string(action);
                        advanceToNextValidPlayer(game);
                    } catch (const std::exception& e) {
                        errorManager.showError(e.what());
                        lastActionTextStr = "Error: " + std::string(e.what());
                    }

                } else if (state == GameState::InGame) {
                    errorManager.showError("Invalid action key.");
                }
            }

            if (showTargetPopup && event.type == sf::Event::KeyPressed) {
                int index = -1;
                switch (event.key.code) {
                    case sf::Keyboard::Num1: index = 0; break;
                    case sf::Keyboard::Num2: index = 1; break;
                    case sf::Keyboard::Num3: index = 2; break;
                    case sf::Keyboard::Num4: index = 3; break;
                    case sf::Keyboard::Num5: index = 4; break;
                    case sf::Keyboard::Num6: index = 5; break;
                    case sf::Keyboard::Escape:
                        showTargetPopup = false;
                        errorManager.showError("Target selection canceled.");
                        continue;
                }

                if (index >= 0 && index < static_cast<int>(validTargets.size())) {
                    Player* target = validTargets[index];
                    Player& actor = game.currentPlayer();

                    try {
                        switch (pendingAction) {
                            case 4: actor.arrest(*target, game); break;
                            case 5: actor.sanction(*target, game); break;
                            case 6: actor.coup(*target, game); break;
                        }
        
                        lastActionTextStr = actor.getName() + " used action " + std::to_string(pendingAction) + " on " + target->getName();
                        pendingAction = -1;
                        showTargetPopup = false;
                        advanceToNextValidPlayer(game);

                    } catch (const std::exception& e) {
                        errorManager.showError(e.what());
                        lastActionTextStr = std::string("Error: ") + e.what();
                    }
                } else {
                    errorManager.showError("Invalid target number.");
                }
                continue;
            }


        }

        window.clear(sf::Color::White);

        if (state == GameState::Welcome) {
            drawWelcomeScreen(window, font, titleText, startButton, startText);
            errorManager.draw(window);
        }

        else if (state == GameState::SelectPlayerCount) {
            drawSelectPlayerCountScreen(window, font, selectText, playerCountText,
                                        minusButton, plusButton, continueButton, continueText,
                                        minusText, plusText,playerCount);
            errorManager.draw(window);
        }
        
        else if (state == GameState::EnterNames) {
            drawEnterNamesScreen(window, font, enterNameTitle, nameInputText,
                         addPlayerButton, addPlayerButtonText,
                         startGameButton, startGameButtonText,
                         addedPlayersListText,
                         playerNames, currentNameInput, playerCount);
            errorManager.draw(window);
        }
        
        
        if (state == GameState::ShowRoles) {
            drawShowRolesScreen(window, font, game.getPlayers(), continueButton, continueText);
            errorManager.draw(window);
        }
        
        if (state == GameState::InGame) {
            drawInGameScreen(window, font, game, lastActionText, lastActionTextStr);
            if (showTargetPopup) {
                drawTargetPopup(window, font, validTargets);
            }
            errorManager.draw(window);
        }  
    
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

        // עיגול לשחקן
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

void handleGuiActionInput(Game& game, sf::Text& lastActionText, sf::Keyboard::Key key) {
    Player& current = game.currentPlayer();
    int action = -1;

    switch (key) {
        case sf::Keyboard::Num1: action = 1; break;
        case sf::Keyboard::Num2: action = 2; break;
        case sf::Keyboard::Num3: action = 3; break;
        case sf::Keyboard::Num4: action = 4; break;
        case sf::Keyboard::Num5: action = 5; break;
        case sf::Keyboard::Num6: action = 6; break;
        case sf::Keyboard::Num7: action = 7; break;
        default: return; // Ignore others
    }

    try {
        switch (action) {
            case 1:
                current.gather(game);
                lastActionText.setString("Last action: Gather");
                break;
            case 2:
                game.attemptTax(&current);
                lastActionText.setString("Last action: Tax");
                break;
            case 3:
                game.attemptBribe(&current);
                lastActionText.setString("Last action: Bribe");
                break;
            case 4:
                current.arrest(*game.selectTarget(&current), game);
                lastActionText.setString("Last action: Arrest");
                break;
            case 5:
                current.sanction(*game.selectTarget(&current), game);
                lastActionText.setString("Last action: Sanction");
                break;
            case 6:
                current.coup(*game.selectTarget(&current), game);
                lastActionText.setString("Last action: Coup");
                break;
            case 7:
                if (current.getRole() == "Baron") {
                    current.invest(game);
                    lastActionText.setString("Last action: Invest");
                } else {
                    lastActionText.setString("Invalid: Only Baron can invest.");
                    return;  // Don't advance turn
                }
                break;
            default:
                lastActionText.setString("Invalid action number");
                return;  // Don't advance turn
        }
        game.nextTurn();
    } catch (const std::exception& e) {
        lastActionText.setString(std::string("Action failed: ") + e.what());
    }
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
