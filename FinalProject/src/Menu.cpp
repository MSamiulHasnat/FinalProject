#include "Menu.h"
#include <iostream>
using namespace std;

// Global variable to store the clicked action
static int sMenuAction = 0;

Menu::Menu(float width, float height) {
    // 1. Load Font
    if (!mFont.loadFromFile("assets/fonts/SupercellMagic.ttf") && 
        !mFont.loadFromFile("FinalProject/assets/fonts/SupercellMagic.ttf")) {
         cerr << "CRITICAL ERROR: SupercellMagic.ttf not found!" <<  endl;
    }

    // 2. Setup Title
    mTitleText.setFont(mFont);
    mTitleText.setString("TIME SURVIVOR");
    mTitleText.setCharacterSize(60);
    mTitleText.setFillColor(sf::Color::Cyan);
    sf::FloatRect textRect = mTitleText.getLocalBounds();
    mTitleText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    mTitleText.setPosition(width / 2.0f, height / 6.0f);

    // 3. Setup Buttons
    float btnWidth = 300.0f;
    float btnHeight = 60.0f;
    float startY = height / 3.0f;
    float gap = 80.0f;

    initButton("START GAME", width / 2.0f, startY, btnWidth, btnHeight, []() { sMenuAction = 1; });
    initButton("LEVEL 1", width / 2.0f, startY + gap, btnWidth, btnHeight, []() { sMenuAction = 1; });
    initButton("LEVEL 2", width / 2.0f, startY + gap * 2, btnWidth, btnHeight, []() { sMenuAction = 2; });
    initButton("LEVEL 3", width / 2.0f, startY + gap * 3, btnWidth, btnHeight, []() { sMenuAction = 3; });
    initButton("QUIT", width / 2.0f, startY + gap * 4, btnWidth, btnHeight, []() { sMenuAction = -1; });
}

Menu::~Menu() {}

void Menu::initButton(const std::string& label, float x, float y, float width, float height, std::function<void()> callback) {
    Button btn;
    btn.shape.setSize(sf::Vector2f(width, height));
    btn.shape.setOrigin(width / 2.0f, height / 2.0f);
    btn.shape.setPosition(x, y);
    btn.shape.setFillColor(sf::Color(50, 50, 50)); // Dark Gray
    btn.shape.setOutlineThickness(2.0f);
    btn.shape.setOutlineColor(sf::Color::White);

    btn.text.setFont(mFont);
    btn.text.setString(label);
    btn.text.setCharacterSize(24);
    btn.text.setFillColor(sf::Color::White);
    sf::FloatRect textRect = btn.text.getLocalBounds();
    btn.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    btn.text.setPosition(x, y);

    btn.onClick = callback;
    mButtons.push_back(btn);
}

void Menu::render(sf::RenderWindow& window) {
    window.draw(mTitleText);
    for (auto& btn : mButtons) {
        drawRoundedButton(window, btn);
    }
}

void Menu::drawRoundedButton(sf::RenderWindow& window, Button& btn) {
    // Simple rounded effect using circles on corners
    float radius = 15.0f;
    sf::Vector2f pos = btn.shape.getPosition();
    sf::Vector2f size = btn.shape.getSize();
    sf::Color color = btn.isHovered ? sf::Color(100, 100, 100) : sf::Color(50, 50, 50);

    // Main body (cross shape)
    sf::RectangleShape rect1(sf::Vector2f(size.x - 2 * radius, size.y));
    rect1.setOrigin(rect1.getSize().x / 2.0f, rect1.getSize().y / 2.0f);
    rect1.setPosition(pos);
    rect1.setFillColor(color);

    sf::RectangleShape rect2(sf::Vector2f(size.x, size.y - 2 * radius));
    rect2.setOrigin(rect2.getSize().x / 2.0f, rect2.getSize().y / 2.0f);
    rect2.setPosition(pos);
    rect2.setFillColor(color);

    // Corners
    sf::CircleShape corner(radius);
    corner.setFillColor(color);
    
    // Top Left
    corner.setPosition(pos.x - size.x / 2.0f, pos.y - size.y / 2.0f);
    window.draw(corner);
    // Top Right
    corner.setPosition(pos.x + size.x / 2.0f - 2 * radius, pos.y - size.y / 2.0f);
    window.draw(corner);
    // Bottom Left
    corner.setPosition(pos.x - size.x / 2.0f, pos.y + size.y / 2.0f - 2 * radius);
    window.draw(corner);
    // Bottom Right
    corner.setPosition(pos.x + size.x / 2.0f - 2 * radius, pos.y + size.y / 2.0f - 2 * radius);
    window.draw(corner);

    window.draw(rect1);
    window.draw(rect2);
    window.draw(btn.text);
}

int Menu::handleInput(sf::RenderWindow& window) {
    sMenuAction = 0;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    bool isClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);

    // Debounce click (simple)
    static bool wasPressed = false;
    bool clickedNow = isClick && !wasPressed;
    wasPressed = isClick;

    for (auto& btn : mButtons) {
        if (btn.shape.getGlobalBounds().contains(worldPos)) {
            btn.isHovered = true;
            if (clickedNow && btn.onClick) {
                btn.onClick();
            }
        } else {
            btn.isHovered = false;
        }
    }
    return sMenuAction;
}