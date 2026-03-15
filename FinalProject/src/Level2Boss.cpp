#include "Level2Boss.h"
#include <iostream>
#include <cstdlib> 
#include <cmath> 
#include <fstream>

using namespace std;

// Helper
bool fileExistsL2(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

// =========================================================
// STATIC MEMBER DEFINITIONS
// =========================================================
sf::Texture Level2Boss::sAttack1Sheet, Level2Boss::sAttack2Sheet, Level2Boss::sAttack3Sheet;
sf::Texture Level2Boss::sDeadSheet, Level2Boss::sHurtSheet, Level2Boss::sIdleSheet;
sf::Texture Level2Boss::sJumpSheet, Level2Boss::sRunSheet, Level2Boss::sRunAttackSheet, Level2Boss::sWalkSheet;

vector<sf::IntRect> Level2Boss::sAttack1Frames, Level2Boss::sAttack2Frames, Level2Boss::sAttack3Frames;
vector<sf::IntRect> Level2Boss::sDeadFrames, Level2Boss::sHurtFrames, Level2Boss::sIdleFrames;
vector<sf::IntRect> Level2Boss::sJumpFrames, Level2Boss::sRunFrames, Level2Boss::sRunAttackFrames, Level2Boss::sWalkFrames;

bool Level2Boss::sTexturesLoaded = false;

// =========================================================
// CONSTRUCTOR
// =========================================================
Level2Boss::Level2Boss(float startX, float startY, float minX, float maxX, bool isMiniBoss) 
    : mIsMiniBoss(isMiniBoss)
{
    mWorldPosition = sf::Vector2f(startX, startY);
    mHitboxWidth = 60.0f; // Adjusted for 128x128 sprite
    mHitboxHeight = 100.0f;

    // Patrol Settings
    mPatrolMinX = minX;
    mPatrolMaxX = maxX;

    // Physics & Offsets
    if (mIsMiniBoss) {
        mSpriteScale = 1.0f; // Normal size for mini boss
        mHealth = 80;
        mMaxHealth = 80;
        mHitboxWidth = 40.0f;
        mHitboxHeight = 66.0f;
    } else {
        mSpriteScale = 1.5f; // Scaled up to 1.5x
        mHealth = 200; // Boss Health
        mMaxHealth = 200;
        mHitboxWidth = 60.0f; 
        mHitboxHeight = 100.0f;
    }
    
    mGroundOffset = 64.0f * mSpriteScale; 
    mPlatformOffset = 64.0f * mSpriteScale;

    // Initialize AI Timers
    mJumpCooldown = 0.0f;
    mAttackCooldown = 0.0f;
    mSearchTimer = 0.0f;

    mPatrolPauseTimer = 0.0f;
    mLastKnownPlayerPos = mWorldPosition;

    // Load Textures (ONLY ONCE)
    if (!sTexturesLoaded) {
        loadAndSlice(sAttack1Sheet, "Attack_1.png", sAttack1Frames, 6);
        loadAndSlice(sAttack2Sheet, "Attack_2.png", sAttack2Frames, 4);
        loadAndSlice(sAttack3Sheet, "Attack_3.png", sAttack3Frames, 5);
        loadAndSlice(sDeadSheet, "Dead.png", sDeadFrames, 2);
        loadAndSlice(sHurtSheet, "Hurt.png", sHurtFrames, 2);
        loadAndSlice(sIdleSheet, "Idle.png", sIdleFrames, 8);
        loadAndSlice(sJumpSheet, "Jump.png", sJumpFrames, 11);
        loadAndSlice(sRunSheet, "Run.png", sRunFrames, 9);
        loadAndSlice(sRunAttackSheet, "Run+Attack.png", sRunAttackFrames, 7);
        loadAndSlice(sWalkSheet, "Walk.png", sWalkFrames, 11);
        
        sTexturesLoaded = true;
    }

    if (!sIdleFrames.empty()) {
        mSprite.setTexture(sIdleSheet);
        mSprite.setTextureRect(sIdleFrames[0]);
    }
    mSprite.setScale(mSpriteScale, mSpriteScale);
    // Center origin
    mSprite.setOrigin(64.0f, 64.0f);

    mIsFacingRight = true;
    mState = EnemyState::PATROL;
}

Level2Boss::~Level2Boss() {}

void Level2Boss::loadAndSlice(sf::Texture& texture, string filename, vector<sf::IntRect>& frameVector, int frameCount) {
    string base = "assets/enemy/Level2EnemyBoss/" + filename;
    string paths[] = {
        base,
        "FinalProject/" + base,
        "../" + base
    };

    bool loaded = false;
    for (const auto& p : paths) {
        if (fileExistsL2(p)) {
            if (texture.loadFromFile(p)) {
                loaded = true;
                break;
            }
        }
    }

    if (!loaded) {
         cout << "Failed to load texture: " << base << endl;
    }
    int frameWidth = 128; int frameHeight = 128;
    frameVector.clear();
    for (int i = 0; i < frameCount; i++) {
        frameVector.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
}

void Level2Boss::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    if (mState == EnemyState::DEAD) return;

    float dt = deltaTime.asSeconds();
    mElapsedTime += dt;
    mDamageCooldown -= dt;
    mAttackCooldown -= dt;
    mJumpCooldown -= dt;

    // Gravity
    if (!mIsOnGround) {
        mVelocity.y += 981.0f * dt;
    }
    mWorldPosition += mVelocity * dt;

    // --- AI LOGIC ---
    float distToPlayer = std::fabs(playerPos.x - mWorldPosition.x);
    float yDiff = std::fabs(playerPos.y - mWorldPosition.y);
    bool playerInZone = (playerPos.x >= mPatrolMinX && playerPos.x <= mPatrolMaxX);

    // 1. Check Death
    if (mHealth <= 0) {
        mState = EnemyState::DEAD;
        mCurrentFrame = 0;
        mElapsedTime = 0;
    }
    // 2. Hurt State
    else if (mDamageCooldown > 0.2f) { // Stunned briefly
        mState = EnemyState::HURT;
    }
    // 3. Attack Logic
    else if (playerInZone && distToPlayer < 100.0f && yDiff < 100.0f && mAttackCooldown <= 0) {
        mState = EnemyState::ATTACK;
        mAttackCooldown = 1.5f; // Cooldown
        mCurrentFrame = 0;
        mElapsedTime = 0;
        mVelocity.x = 0;
    }
    // 4. Chase Logic (Only within bounds)
    else if (playerInZone && distToPlayer < 600.0f && mState != EnemyState::ATTACK) {
        mState = EnemyState::CHASE;
        if (playerPos.x > mWorldPosition.x) {
            mVelocity.x = 150.0f;
            mIsFacingRight = true;
        } else {
            mVelocity.x = -150.0f;
            mIsFacingRight = false;
        }
    }
    // 5. Patrol Logic
    else if (mState != EnemyState::ATTACK) {
        mState = EnemyState::PATROL;
        if (mIsFacingRight) {
            mVelocity.x = 100.0f;
            if (mWorldPosition.x > mPatrolMaxX) {
                mIsFacingRight = false;
                mVelocity.x = -100.0f;
            }
        } else {
            mVelocity.x = -100.0f;
            if (mWorldPosition.x < mPatrolMinX) {
                mIsFacingRight = true;
                mVelocity.x = 100.0f;
            }
        }
    }

    // Constrain to Platform
    if (mWorldPosition.x < mPatrolMinX) mWorldPosition.x = mPatrolMinX;
    if (mWorldPosition.x > mPatrolMaxX) mWorldPosition.x = mPatrolMaxX;

    // Update Animation
    updateAnimation(deltaTime);

    // Update Sprite
    mSprite.setPosition(mWorldPosition);
    if (mIsFacingRight) {
        mSprite.setScale(mSpriteScale, mSpriteScale);
    } else {
        mSprite.setScale(-mSpriteScale, mSpriteScale);
    }

    // Health Bar
    // Sprite is 128x128. Origin is Center (64,64).
    // Visual Top is mWorldPosition.y - (64 * mSpriteScale).
    // We want the bar slightly above that.
    float visualTop = mWorldPosition.y - (64.0f * mSpriteScale);
    
    mHealthBarBack.setPosition(mWorldPosition.x - 25, visualTop - 15.0f);
    mHealthBarFront.setPosition(mWorldPosition.x - 25, visualTop - 15.0f);
    
    float hpPct = (float)mHealth / (float)mMaxHealth;
    if (hpPct < 0) hpPct = 0;
    mHealthBarFront.setSize(sf::Vector2f(50.0f * hpPct, 5.0f));
}

sf::FloatRect Level2Boss::getBodyHitbox() const {
    // Scale hitbox with sprite
    float scaledW = mHitboxWidth * mSpriteScale;
    float scaledH = mHitboxHeight * mSpriteScale;
    
    return sf::FloatRect(
        mWorldPosition.x - scaledW / 2.0f,
        mWorldPosition.y - scaledH / 2.0f,
        scaledW,
        scaledH
    );
}

sf::FloatRect Level2Boss::getFeetHitbox() const {
    float scaledW = mHitboxWidth * mSpriteScale;
    float scaledH = mHitboxHeight * mSpriteScale;
    float halfHeight = scaledH / 2.0f;
    float feetHeight = 10.0f;
    
    return sf::FloatRect(
        mWorldPosition.x - scaledW / 4.0f, 
        mWorldPosition.y + halfHeight - feetHeight,
        scaledW / 2.0f,
        feetHeight
    );
}

void Level2Boss::updateAnimation(sf::Time deltaTime) {
    float frameDuration = 0.1f;
    int frameCount = 1;
    vector<sf::IntRect>* currentFrames = &sIdleFrames;

    switch (mState) {
    case EnemyState::IDLE:
        currentFrames = &sIdleFrames;
        frameCount = 8;
        break;
    case EnemyState::PATROL:
        currentFrames = &sWalkFrames;
        frameCount = 11;
        break;
    case EnemyState::CHASE:
        currentFrames = &sRunFrames;
        frameCount = 9;
        break;
    case EnemyState::ATTACK:
        // Randomize attack animation or cycle? For now use Attack 1
        currentFrames = &sAttack1Frames;
        frameCount = 6;
        frameDuration = 0.08f; // Faster attack
        break;
    case EnemyState::HURT:
        currentFrames = &sHurtFrames;
        frameCount = 2;
        break;
    case EnemyState::DEAD:
        currentFrames = &sDeadFrames;
        frameCount = 2;
        break;
    default:
        currentFrames = &sIdleFrames;
        frameCount = 8;
        break;
    }

    if (mElapsedTime >= frameDuration) {
        mCurrentFrame++;
        mElapsedTime = 0;
        if (mCurrentFrame >= frameCount) {
            if (mState == EnemyState::ATTACK || mState == EnemyState::HURT) {
                mState = EnemyState::IDLE; // Return to idle after action
                mCurrentFrame = 0;
            } else if (mState == EnemyState::DEAD) {
                mCurrentFrame = frameCount - 1; // Stay on last frame
            } else {
                mCurrentFrame = 0; // Loop
            }
        }
    }

    if (!currentFrames->empty()) {
        mSprite.setTextureRect((*currentFrames)[mCurrentFrame]);
        // Update texture if needed (since we have different sheets)
        if (mState == EnemyState::IDLE) mSprite.setTexture(sIdleSheet);
        else if (mState == EnemyState::PATROL) mSprite.setTexture(sWalkSheet);
        else if (mState == EnemyState::CHASE) mSprite.setTexture(sRunSheet);
        else if (mState == EnemyState::ATTACK) mSprite.setTexture(sAttack1Sheet);
        else if (mState == EnemyState::HURT) mSprite.setTexture(sHurtSheet);
        else if (mState == EnemyState::DEAD) mSprite.setTexture(sDeadSheet);
    }
}
