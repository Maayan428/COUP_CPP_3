//maayan428@gmail.com

// GUIErrorManager.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class GUIErrorManager {
private:
sf::Text errorText;
sf::Clock timer;
bool visible = false;
float duration = 3.0f;

public:
    GUIErrorManager() {}

    void init(const sf::Font& font) {
        errorText.setFont(font);
        errorText.setCharacterSize(20);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(50, 550);
    }

    void showError(const std::string& message, float seconds = 3.0f) {
        errorText.setString(" Error: " + message);
        errorText.setPosition(20, 20);
        timer.restart();
        visible = true;
        duration = seconds;
    }

    void draw(sf::RenderWindow& window) {
        if (visible && timer.getElapsedTime().asSeconds() < duration) {
            window.draw(errorText);
        } else {
            visible = false;
        }
    }

    void clear() {
        visible = false;
        errorText.setString("");
    }

    bool isVisible() const {
        return visible;
    }
};