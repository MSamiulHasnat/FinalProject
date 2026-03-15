#pragma once
#include "Enemy.h"
#include <vector>
#include <string>

using namespace std;

class Level1Boss : public Enemy {
public:
    Level1Boss(float startX, float startY, bool isMiniBoss = false);
    ~Level1Boss();

    void update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) override;
    // Override render to fix health bar alignment
    void render(sf::RenderWindow& window, sf::Vector2f cameraOffset) override;

private:
    // STATIC Textures (Right & Left)
    static sf::Texture sIdleRightSheet, sIdleLeftSheet;
    static sf::Texture sFlyingRightSheet, sFlyingLeftSheet;
    static sf::Texture sAttackRightSheet, sAttackLeftSheet;
    static sf::Texture sHurtRightSheet, sHurtLeftSheet;
    static sf::Texture sDeathRightSheet, sDeathLeftSheet;

    // STATIC Frame Vectors
    static vector<sf::IntRect> sIdleRightFrames, sIdleLeftFrames;
    static vector<sf::IntRect> sFlyingRightFrames, sFlyingLeftFrames;
    static vector<sf::IntRect> sAttackRightFrames, sAttackLeftFrames;
    static vector<sf::IntRect> sHurtRightFrames, sHurtLeftFrames;
    static vector<sf::IntRect> sDeathRightFrames, sDeathLeftFrames;

    static bool sTexturesLoaded;

    // Helper to load sprite strips
    void updateAnimation(sf::Time deltaTime);

    // Boss Specific AI
    float mAttackCooldown;
    float mAttackRange;
    float mChaseSpeed;
    bool mIsMiniBoss; // New flag
    
    // Initial Spawn point to leash back to if needed
    sf::Vector2f mSpawnPoint;
};