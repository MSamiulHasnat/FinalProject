#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

using namespace std;

// The state machine for the player
enum class PlayerState {
    IDLE,
    WALK,
    RUN,
    JUMP,
    FALL,
    SLIDE,
    WALK_ROLL,
    RUN_ROLL,
    ATTACK,
    ATTACK2,
    ATTACK3
};

// The direction the player is facing
enum class Direction {
    LEFT,
    RIGHT
};

class Player {
public:
    Player();
    ~Player();

    void handlePlayerInput(sf::Time deltaTime);
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window, sf::Vector2f cameraOffset);

    // --- CLEANED UP HITBOX FUNCTIONS ---
    sf::FloatRect getFeetHitbox() const;  // For landing on platforms
    sf::FloatRect getBodyHitbox() const;  // For taking damage
    void handleCollision(const sf::FloatRect& platformBounds);

    void setIsOnGround(bool onGround) { mIsOnGround = onGround; }

    void attack1();
    void attack2();
    void attack3();

    sf::Vector2f getVelocity() const { return mVelocity; }
    sf::Vector2f getWorldPosition() const { return mWorldPosition; }

    void setWorldPosition(sf::Vector2f pos) { mWorldPosition = pos; }
    float getVisualWidth() const { return 96.0f * 4.0f; }

private:
    sf::Sprite mSprite;
    PlayerState mState;
    Direction mDirection;

    sf::Vector2f mVelocity;
    float mWalkSpeed;
    float mRunSpeed;
    float mGravity;
    float mJumpSpeed;
    bool mIsOnGround;

    // --- Textures ---
    sf::Texture mIdleSheetRight, mIdleSheetLeft;
    sf::Texture mAttackSheetRight, mAttackSheetLeft;
    sf::Texture mWalkSheetRight, mWalkSheetLeft;
    sf::Texture mRunSheetRight, mRunSheetLeft;
    sf::Texture mJumpSheetRight, mJumpSheetLeft;
    sf::Texture mFallSheetRight, mFallSheetLeft;
    sf::Texture mSlideSheetRight, mSlideSheetLeft;
    sf::Texture mWalkRollSheetRight, mWalkRollSheetLeft;
    sf::Texture mRunRollSheetRight, mRunRollSheetLeft;
    sf::Texture mAttack2SheetRight, mAttack2SheetLeft;
    sf::Texture mAttack3SheetRight, mAttack3SheetLeft;

    // --- Frame Vectors ---
    vector<sf::IntRect> mIdleFramesRight, mIdleFramesLeft;
    vector<sf::IntRect> mAttackFramesRight, mAttackFramesLeft;
    vector<sf::IntRect> mWalkFramesRight, mWalkFramesLeft;
    vector<sf::IntRect> mRunFramesRight, mRunFramesLeft;
    vector<sf::IntRect> mJumpFramesRight, mJumpFramesLeft;
    vector<sf::IntRect> mFallFramesRight, mFallFramesLeft;
    vector<sf::IntRect> mSlideFramesRight, mSlideFramesLeft;
    vector<sf::IntRect> mWalkRollFramesRight, mWalkRollFramesLeft;
    vector<sf::IntRect> mRunRollFramesRight, mRunRollFramesLeft;
    vector<sf::IntRect> mAttack2FramesRight, mAttack2FramesLeft;
    vector<sf::IntRect> mAttack3FramesRight, mAttack3FramesLeft;

    // --- Animation & Timers ---
    int mCurrentFrame;
    float mFrameTime;
    float mElapsedTime;
    float mWalkHoldTimer;
    const float mTimeToRun = 1.5f;
    bool mCanSlide;

    // Helper function
    void sliceSpriteSheet(const sf::Texture& texture, int frameWidth, int frameHeight, int frameCount, vector<sf::IntRect>& frames, bool isReversed);

    sf::Vector2f mWorldPosition;
};