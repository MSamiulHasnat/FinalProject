#pragma once
#include "Enemy.h"
#include <vector>
#include <string>

using namespace std;

class Level2Boss : public Enemy {
public:
    Level2Boss(float startX, float startY, float minX, float maxX, bool isMiniBoss = false);
    ~Level2Boss();

    void update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) override;
    
    // Override hitboxes for Center Origin
    sf::FloatRect getBodyHitbox() const override;
    sf::FloatRect getFeetHitbox() const override;

private:
    bool mIsMiniBoss;
    // Textures & Frames (STATIC for optimization)
    static sf::Texture sAttack1Sheet, sAttack2Sheet, sAttack3Sheet;
    static sf::Texture sDeadSheet, sHurtSheet, sIdleSheet;
    static sf::Texture sJumpSheet, sRunSheet, sRunAttackSheet, sWalkSheet;

    static vector<sf::IntRect> sAttack1Frames, sAttack2Frames, sAttack3Frames;
    static vector<sf::IntRect> sDeadFrames, sHurtFrames, sIdleFrames;
    static vector<sf::IntRect> sJumpFrames, sRunFrames, sRunAttackFrames, sWalkFrames;

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
