#ifndef SFML_GUI_HPP
#define SFML_GUI_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Game.hpp" 
#include "Player.hpp"

// Different screens of the game
enum class GameState {
    Welcome,
    SelectPlayerCount,
    EnterNames,
    ShowRoles,
    InGame
};

// Accesses the main GUI page
void launchGui();

// Creates the Welcome screen
void drawWelcomeScreen(
    sf::RenderWindow& window,
    sf::Font& font,
    sf::Text& titleText,
    sf::RectangleShape& startButton,
    sf::Text& startText
);

void drawSelectPlayerCountScreen(sf::RenderWindow& window, sf::Font& font,
    sf::Text& selectText, sf::Text& playerCountText,
    sf::RectangleShape& minusButton, sf::RectangleShape& plusButton,
    sf::RectangleShape& continueButton, sf::Text& continueText,
    sf::Text& minusText, sf::Text& plusText,  
    int playerCount);

void drawEnterNamesScreen(sf::RenderWindow& window, sf::Font& font,
    sf::Text& enterNameTitle, sf::Text& nameInputText,
    sf::RectangleShape& addPlayerButton, sf::Text& addPlayerButtonText,
    sf::RectangleShape& startGameButton, sf::Text& startGameButtonText,
    sf::Text& addedPlayersListText,
    const std::vector<std::string>& playerNames,
    const std::string& currentNameInput,
    int playerCount);

void drawShowRolesScreen(sf::RenderWindow& window, sf::Font& font,
    const std::vector<Player*>& players,
    sf::RectangleShape& continueButton, sf::Text& continueText);

void drawInGameScreen(sf::RenderWindow& window, sf::Font& font, Game& game, sf::Text& lastActionText, const std::string& lastActionTextStr);

void drawTargetPopup(sf::RenderWindow& window, sf::Font& font, const std::vector<Player*>& targets);

void advanceToNextValidPlayer(Game& game);

#endif // SFML_GUI_HPP