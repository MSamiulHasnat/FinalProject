#include "Enemy.h"

Enemy::Enemy()
    : mState(EnemyState::IDLE)
    , mWorldPosition(0, 0)
    , mVelocity(0, 0)
    , mGravity(1800.0f)
    , mIsOnGround(false)
    , mIsFacingRight(true)
    , mCurrentFrame(0)
    , mFrameTime(0.1f)
    , mElapsedTime(0.0f)
    , mHitboxWidth(64.0f)
    , mHitboxHeight(64.0f)
    , mGroundOffset(0.0f)
    , mPlatformOffset(0.0f)
    , mSpriteScale(4.0f)
{
}

Enemy::~Enemy() {}

// --- CHANGE: Added platforms parameter ---
void Enemy::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    mVelocity.y += mGravity * deltaTime.asSeconds();
    if (mVelocity.y > 1500) mVelocity.y = 1500;

    mWorldPosition += mVelocity * deltaTime.asSeconds();

    if (mWorldPosition.y > 1000) {
        mState = EnemyState::DEAD;
    }
}

void Enemy::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    mSprite.setPosition(
        mWorldPosition.x - cameraOffset.x,
        mWorldPosition.y - cameraOffset.y
    );
    window.draw(mSprite);
}

sf::FloatRect Enemy::getFeetHitbox() const {
    float fullHeight = mHitboxHeight * mSpriteScale;
    float feetHeight = 10.0f;

    return sf::FloatRect(
        mWorldPosition.x + (mHitboxWidth * mSpriteScale * 0.25f),
        mWorldPosition.y + fullHeight - feetHeight,
        mHitboxWidth * mSpriteScale * 0.5f,
        feetHeight
    );
}

sf::FloatRect Enemy::getBodyHitbox() const {
    return sf::FloatRect(
        mWorldPosition.x,
        mWorldPosition.y,
        mHitboxWidth * mSpriteScale,
        mHitboxHeight * mSpriteScale
    );
}

void Enemy::handleCollision(const sf::FloatRect& platformBounds) {
    sf::FloatRect feet = getFeetHitbox();

    if (feet.intersects(platformBounds) && mVelocity.y > 0) {
        mIsOnGround = true;
        mVelocity.y = 0;

        float fullHeight = mHitboxHeight * mSpriteScale;

        if (platformBounds.top == (900.0f - 185.0f)) {
            mWorldPosition.y = (platformBounds.top - fullHeight) + mGroundOffset;
        }
        else {
            mWorldPosition.y = (platformBounds.top - fullHeight) + mPlatformOffset;
        }
    }
}

void Enemy::takeDamage(int damage) {
    if (mState != EnemyState::DEAD && mState != EnemyState::HURT) {
        mState = EnemyState::HURT;
        mCurrentFrame = 0;
    }
}