#include "Menu.h"
#include <iostream>
using namespace std;

Menu::Menu(float width, float height) {
    // 1. ROBUST FONT LOADING
    // Attempt 1: Relative Path (Best Practice)
    if (!mFont.loadFromFile("../assets/fonts/arial.ttf")) {
         cout << "Warning: Could not load local font. Trying absolute path..." <<  endl;

        // Attempt 2: Your Absolute Project Path
        if (!mFont.loadFromFile("C:/Users/Samiul/source/repos/FinalProject/FinalProject/assets/fonts/ARIAL.TTF")) {
             cout << "Warning: Could not load absolute font. Trying System Default..." <<  endl;

            // Attempt 3: Windows System Default (Fallback)
            // This is the "Default Font" logic you requested
            if (!mFont.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                cerr << "CRITICAL ERROR: No fonts found! Text will not display." <<  endl;
                // At this point, text will be invisible, but the game won't crash immediately.
            }
        }
    }

    // 2. Setup Title
    initText(mTitleText, 60, sf::Color::Cyan, "TIME SURVIVOR", width / 2.0f, height / 3.0f);

    // 3. Setup Instructions
    initText(mInstructionText, 30, sf::Color::White, "Press [ENTER] to Start\nPress [ESC] to Quit", width / 2.0f, height / 1.8f);
}

// Add this to Menu.cpp if it is missing!
Menu::~Menu() {
    // Destructor code (can be empty)
}

void Menu::render(sf::RenderWindow& window) {

    // Draw the actual text
    window.draw(mTitleText);
    window.draw(mInstructionText);
}

int Menu::handleInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Enter) {
        return 1; // Code for "Start Game"
    }
    if (key == sf::Keyboard::Escape) {
        return -1; // Code for "Exit Application"
    }
    return 0; // No relevant key pressed
}

// Helper function to center and setup text
void Menu::initText(sf::Text& text, int size, sf::Color color,  string content, float x, float y) {
    text.setFont(mFont);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);

    // Center the origin of the text object so it aligns perfectly
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(x, y);
}