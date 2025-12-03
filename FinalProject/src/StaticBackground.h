#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class StaticBackground {
public:
    StaticBackground(float windowWidth, float windowHeight, string texturePath);
    ~StaticBackground();

    void render(sf::RenderWindow& window, sf::Vector2f cameraOffset);

    // --- ADD THIS FUNCTION ---
    float getTextureWidth() const {
        return static_cast<float>(mTexture.getSize().x);
    }

private:
    sf::Texture mTexture;
    sf::Sprite mSprite;

    float mWindowWidth;
    float mWindowHeight;
};