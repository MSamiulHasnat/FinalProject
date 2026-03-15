#pragma once
#include <SFML/Graphics.hpp>
#include <vector> // Required for std::vector

enum class EnemyState {
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    HURT,
    DYING,
    DEAD,
    SEARCH
};

class Enemy {
public:
    Enemy();
    virtual ~Enemy();

    // --- CHANGE: Added 'platforms' to the parameters ---
    virtual void update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms);

    virtual void render(sf::RenderWindow& window, sf::Vector2f cameraOffset);
    virtual void handleCollision(const sf::FloatRect& platformBounds);

    virtual sf::FloatRect getBodyHitbox() const;
    virtual sf::FloatRect getFeetHitbox() const;

    sf::Vector2f getPosition() const { return mWorldPosition; }
    void setPosition(float x, float y) { mWorldPosition = sf::Vector2f(x, y); }
    bool isDead() const { return mState == EnemyState::DEAD; }
    void takeDamage(int damage);
    int getHealth() const { return mHealth; }
    int getMaxHealth() const { return mMaxHealth; }
    bool isHurt() const { return mState == EnemyState::HURT; }
    bool isAttacking() const { return mState == EnemyState::ATTACK; }

protected:
    int mHealth;
    int mMaxHealth;
    sf::Sprite mSprite;
    
    // Health Bar
    sf::RectangleShape mHealthBarBack;
    sf::RectangleShape mHealthBarFront;

    sf::Vector2f mWorldPosition;
    sf::Vector2f mVelocity;
    EnemyState mState;
    bool mIsFacingRight;
    float mDamageCooldown; // Invincibility timer
    float mGravity;
    bool mIsOnGround;
    int mCurrentFrame;
    float mFrameTime;
    float mElapsedTime;
    float mHitboxWidth;
    float mHitboxHeight;
    float mGroundOffset;
    float mPlatformOffset;
    float mSpriteScale;
};