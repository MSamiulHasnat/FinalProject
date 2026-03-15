#pragma once
#include "Enemy.h"
#include <vector>
#include <string>
#include <cmath> 

using namespace std;

class Level2Enemy1 : public Enemy {
public:
    Level2Enemy1(float startX, float startY);
    ~Level2Enemy1();

    void update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) override;

private:
    // Textures & Frames (STATIC for optimization)
    static sf::Texture sAttackRightSheet, sAttackLeftSheet;
    static sf::Texture sHurtRightSheet, sHurtLeftSheet;
    static sf::Texture sIdleRightSheet, sIdleLeftSheet;
    static sf::Texture sJumpRightSheet, sJumpLeftSheet;
    static sf::Texture sRunRightSheet, sRunLeftSheet;
    static sf::Texture sWalkRightSheet, sWalkLeftSheet;

    static vector<sf::IntRect> sAttackRightFrames, sAttackLeftFrames;
    static vector<sf::IntRect> sHurtRightFrames, sHurtLeftFrames;
    static vector<sf::IntRect> sIdleRightFrames, sIdleLeftFrames;
    static vector<sf::IntRect> sJumpRightFrames, sJumpLeftFrames;
    static vector<sf::IntRect> sRunRightFrames, sRunLeftFrames;
    static vector<sf::IntRect> sWalkRightFrames, sWalkLeftFrames;

    static bool sTexturesLoaded;

    void loadAndSlice(sf::Texture& texture, string filename, vector<sf::IntRect>& frameVector, int frames);
    void updateAnimation(sf::Time deltaTime);

    // AI Variables
    float mPatrolMinX;
    float mPatrolMaxX;
    float mJumpCooldown;
    float mAttackCooldown;
    sf::Vector2f mLastKnownPlayerPos;
    float mSearchTimer;
    float mPatrolPauseTimer;
};