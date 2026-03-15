#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class Gateway {
public:
    Gateway(float x, float y);
    ~Gateway();

    void update(sf::Time deltaTime, int score, bool enemiesCleared);
    void render(sf::RenderWindow& window, sf::Vector2f cameraOffset);
    
    bool isActive() const { return mIsActive; }
    sf::FloatRect getHitbox() const;

private:
    sf::Sprite mSprite;
    sf::Texture mTextureOff;
    sf::Texture mTextureOn;
    
    std::vector<sf::IntRect> mFramesOn;
    int mCurrentFrame;
    float mFrameTime;
    float mElapsedTime;
    
    bool mIsActive;
    sf::Vector2f mPosition;
};
