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
    , mHealth(80) // Increased from 50 to 80 to ensure at least 2 hits (Max dmg 40 * 2 = 80)
    , mMaxHealth(80)
    , mDamageCooldown(0.0f)
{
    // Initialize Health Bar
    mHealthBarBack.setSize(sf::Vector2f(50.0f, 6.0f));
    mHealthBarBack.setFillColor(sf::Color::Red);
    mHealthBarBack.setOutlineColor(sf::Color::Black);
    mHealthBarBack.setOutlineThickness(1.0f);

    mHealthBarFront.setSize(sf::Vector2f(50.0f, 6.0f));
    mHealthBarFront.setFillColor(sf::Color::Green);
}

Enemy::~Enemy() {}

// --- CHANGE: Added platforms parameter ---
void Enemy::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    if (mDamageCooldown > 0.0f) {
        mDamageCooldown -= deltaTime.asSeconds();
    }

    mVelocity.y += mGravity * deltaTime.asSeconds();
    if (mVelocity.y > 1500) mVelocity.y = 1500;

    mWorldPosition += mVelocity * deltaTime.asSeconds();

    if (mWorldPosition.y > 1000) {
        mState = EnemyState::DEAD;
    }

    // Update Health Bar
    if (mState != EnemyState::DEAD) {
        float healthPct = static_cast<float>(mHealth) / static_cast<float>(mMaxHealth);
        if (healthPct < 0) healthPct = 0;
        mHealthBarFront.setSize(sf::Vector2f(50.0f * healthPct, 6.0f));
    }
}

void Enemy::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    mSprite.setPosition(
        mWorldPosition.x - cameraOffset.x,
        mWorldPosition.y - cameraOffset.y
    );
    window.draw(mSprite);

    // Draw Health Bar (if not dead)
    if (mState != EnemyState::DEAD) {
        // Center above enemy
        float barX = (mWorldPosition.x - cameraOffset.x) + (mHitboxWidth * mSpriteScale / 2.0f) - 25.0f;
        float barY = (mWorldPosition.y - cameraOffset.y) - 15.0f;

        mHealthBarBack.setPosition(barX, barY);
        mHealthBarFront.setPosition(barX, barY);

        window.draw(mHealthBarBack);
        window.draw(mHealthBarFront);
    }
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
    if (mState == EnemyState::DEAD || mState == EnemyState::DYING) return;
    if (mDamageCooldown > 0.0f) return; // Ignore damage if invincible

    mHealth -= damage;
    mDamageCooldown = 0.5f; // 0.5 second invincibility

    if (mHealth <= 0) {
        mHealth = 0;
        // Play HURT animation one last time before dying
        if (mState != EnemyState::HURT) {
            mState = EnemyState::HURT;
            mCurrentFrame = 0;
        }
    } else if (mState != EnemyState::HURT) {
        mState = EnemyState::HURT;
        mCurrentFrame = 0;
    }
}