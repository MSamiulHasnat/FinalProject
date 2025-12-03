#pragma once
#include <SFML/Graphics.hpp>

class Menu {
public:
    // Constructor: Takes window dimensions to center text
    Menu(float width, float height);
    ~Menu();

    // Draws the text to the provided window
    void render(sf::RenderWindow& window);

    // Checks key presses and returns an action code
    // 0 = No Action, 1 = Start Game, -1 = Exit Game
    int handleInput(sf::Keyboard::Key key);

private:
    sf::Font mFont;
    sf::Text mTitleText;
    sf::Text mInstructionText;

    // Helper to setup text properties to keep code clean
    void initText(sf::Text& text, int size, sf::Color color, std::string content, float x, float y);
};