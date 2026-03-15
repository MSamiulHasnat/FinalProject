#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <functional>

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    std::function<void()> onClick;
    bool isHovered = false;
};

class Menu {
public:
    Menu(float width, float height);
    ~Menu();

    void render(sf::RenderWindow& window);
    
    // Returns action code: 0=None, 1=Start(L1), 2=L2, 3=L3, -1=Quit
    int handleInput(sf::RenderWindow& window);

private:
    sf::Font mFont;
    sf::Text mTitleText;
    std::vector<Button> mButtons;
    sf::Texture mBackgroundTexture; // New background texture
    sf::Sprite mBackgroundSprite;   // New background sprite

    void initButton(const std::string& label, float x, float y, float width, float height, std::function<void()> callback);
    void drawRoundedButton(sf::RenderWindow& window, Button& btn);
};