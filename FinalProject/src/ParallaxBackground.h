#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class ParallaxBackground {
public:
    ParallaxBackground(float windowWidth, float windowHeight, const std::string& levelPath, int layerCount);
    ~ParallaxBackground();

    void render(sf::RenderWindow& window, sf::Vector2f cameraOffset);

private:
    std::vector<sf::Texture> mTextures;
    std::vector<sf::Sprite> mSprites;
    std::vector<float> mParallaxFactors;
    
    float mWindowWidth;
    float mWindowHeight;
};
