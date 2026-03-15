#pragma once
#include "Enemy.h"
#include <vector>
#include <string>

using namespace std;

class Level3Boss : public Enemy {
public:
    Level3Boss(float startX, float startY, float minX, float maxX);
    ~Level3Boss();

    void update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) override;
    
    // Override hitboxes for Center Origin
    sf::FloatRect getBodyHitbox() const override;
    sf::FloatRect getFeetHitbox() const override;
    void render(sf::RenderWindow& window, sf::Vector2f cameraOffset) override;

private:
    // Textures & Frames (STATIC for optimization)
    static sf::Texture sAttackSheet;
    static sf::Texture sDeadSheet, sHurtSheet, sIdleSheet;
    static sf::Texture sWalkSheet;

    static vector<sf::IntRect> sAttackFrames;
    static vector<sf::IntRect> sDeadFrames, sHurtFrames, sIdleFrames;
    static vector<sf::IntRect> sWalkFrames;

    static bool sTexturesLoaded;

    // Helper to load and slice spritesheets
    void loadAndSlice(sf::Texture& texture, const std::string& filename, std::vector<sf::IntRect>& frames, int frameCount);
    
    // State Management
    enum State { IDLE, PATROL, CHASE, ATTACK, HURT, DEAD };
    State mState;
    
    float mStateTimer;
    int mCurrentFrame;
    float mFrameTime;
    float mAnimationSpeed;
    
    // AI Variables
    float mPatrolMinX, mPatrolMaxX;
    float mPatrolPauseTimer;
    float mAttackCooldown;
    bool mIsFacingRight;
    
    // Constants
    const float WALK_SPEED = 100.0f;
    const float CHASE_SPEED = 180.0f;
    const float ATTACK_RANGE = 100.0f; 
    const float DETECTION_RANGE = 600.0f;
    
    void updateAnimation(sf::Time deltaTime);
    void updateAI(sf::Time deltaTime, sf::Vector2f playerPos);
};
