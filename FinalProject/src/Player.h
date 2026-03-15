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
    ATTACK3,
    SITTING_DOWN,
    SITTING_IDLE,
    STANDING_UP,
    DEAD,
    HURT,
    PROTECTION
};

// The direction the player is facing
enum class Direction {
    LEFT,
    RIGHT
};

class Player {
public:
    Player(int characterId = 1);
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

    // Health System
    void takeDamage(int damage);
    int getHealth() const { return mHealth; }
    int getMaxHealth() const { return mMaxHealth; }
    bool isDead() const { return mHealth <= 0; }
    
    // Attack Hitbox
    sf::FloatRect getAttackHitbox() const;
    bool isAttacking() const;
    int getAttackType() const; // 1=Z, 2=X, 3=C

    sf::Vector2f getVelocity() const { return mVelocity; }
    
    // Standardize World Position to always return Top-Left coordinate
    sf::Vector2f getWorldPosition() const { return mWorldPosition; }

    void setWorldPosition(sf::Vector2f pos) { mWorldPosition = pos; }
    
    // Player 1: 96 * 4 = 384 width. Player 2: 128 * 2 = 256 width.
    float getVisualWidth() const { return (mCharacterId == 2) ? 256.0f : 384.0f; }
    // Player 1: 64 * 4 = 256 height. Player 2: 128 * 2 = 256 height.
    float getVisualHeight() const { return 256.0f; }

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
    sf::Texture mSittingDownSheetRight, mSittingDownSheetLeft;
    sf::Texture mSittingIdleSheetRight, mSittingIdleSheetLeft;
    sf::Texture mStandingUpSheetRight, mStandingUpSheetLeft;

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
    vector<sf::IntRect> mSittingDownFramesRight, mSittingDownFramesLeft;
    vector<sf::IntRect> mSittingIdleFramesRight, mSittingIdleFramesLeft;
    vector<sf::IntRect> mStandingUpFramesRight, mStandingUpFramesLeft;

    // Player 2 Specific Frames
    sf::Texture mDeadSheetRight, mDeadSheetLeft;
    sf::Texture mHurtSheetRight, mHurtSheetLeft;
    sf::Texture mProtectionSheetRight, mProtectionSheetLeft;
    
    vector<sf::IntRect> mDeadFramesRight, mDeadFramesLeft;
    vector<sf::IntRect> mHurtFramesRight, mHurtFramesLeft;
    vector<sf::IntRect> mProtectionFramesRight, mProtectionFramesLeft;

    // --- Animation & Timers ---
    int mCurrentFrame;
    float mFrameTime;
    
    // Health
    int mHealth;
    int mMaxHealth;
    float mInvulnerabilityTimer;
    float mHealingTimer; // New: For sitting regeneration
    float mElapsedTime;
    float mWalkHoldTimer;
    const float mTimeToRun = 1.5f;
    bool mCanSlide;
    int mCharacterId;

    // Helper function
    void sliceSpriteSheet(const sf::Texture& texture, int frameWidth, int frameHeight, int frameCount, vector<sf::IntRect>& frames, bool isReversed);

    sf::Vector2f mWorldPosition;
};