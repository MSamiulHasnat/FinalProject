#pragma once
#include <SFML/Graphics.hpp>
#include <vector> // Required for std::vector

enum class EnemyState {
    IDLE,
    PATROL,
    CHASE,
    ATTACK,
    HURT,
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

    sf::FloatRect getBodyHitbox() const;
    sf::FloatRect getFeetHitbox() const;

    void setPosition(float x, float y) { mWorldPosition = sf::Vector2f(x, y); }
    bool isDead() const { return mState == EnemyState::DEAD; }
    void takeDamage(int damage);

protected:
    sf::Sprite mSprite;
    sf::Vector2f mWorldPosition;
    sf::Vector2f mVelocity;
    EnemyState mState;
    bool mIsFacingRight;
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