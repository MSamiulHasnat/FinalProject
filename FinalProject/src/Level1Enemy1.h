#pragma once
#include "Enemy.h"
#include <vector>
#include <string>
#include <cmath> 

using namespace std;

class Level1Enemy1 : public Enemy {
public:
    Level1Enemy1(float startX, float startY);
    ~Level1Enemy1();

    // --- CHANGE: Added platforms parameter ---
    void update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) override;

private:
    // Textures & Frames
    sf::Texture mAttackRightSheet, mAttackLeftSheet;
    sf::Texture mHurtRightSheet, mHurtLeftSheet;
    sf::Texture mIdleRightSheet, mIdleLeftSheet;
    sf::Texture mJumpRightSheet, mJumpLeftSheet;
    sf::Texture mRunRightSheet, mRunLeftSheet;
    sf::Texture mWalkRightSheet, mWalkLeftSheet;

    vector<sf::IntRect> mAttackRightFrames, mAttackLeftFrames;
    vector<sf::IntRect> mHurtRightFrames, mHurtLeftFrames;
    vector<sf::IntRect> mIdleRightFrames, mIdleLeftFrames;
    vector<sf::IntRect> mJumpRightFrames, mJumpLeftFrames;
    vector<sf::IntRect> mRunRightFrames, mRunLeftFrames;
    vector<sf::IntRect> mWalkRightFrames, mWalkLeftFrames;

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