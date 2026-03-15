#include "Level2Enemy1.h"
#include <iostream>
#include <cstdlib> 

using namespace std;

// =========================================================
// STATIC MEMBER DEFINITIONS
// =========================================================
sf::Texture Level2Enemy1::sAttackRightSheet, Level2Enemy1::sAttackLeftSheet;
sf::Texture Level2Enemy1::sHurtRightSheet, Level2Enemy1::sHurtLeftSheet;
sf::Texture Level2Enemy1::sIdleRightSheet, Level2Enemy1::sIdleLeftSheet;
sf::Texture Level2Enemy1::sJumpRightSheet, Level2Enemy1::sJumpLeftSheet;
sf::Texture Level2Enemy1::sRunRightSheet, Level2Enemy1::sRunLeftSheet;
sf::Texture Level2Enemy1::sWalkRightSheet, Level2Enemy1::sWalkLeftSheet;

vector<sf::IntRect> Level2Enemy1::sAttackRightFrames, Level2Enemy1::sAttackLeftFrames;
vector<sf::IntRect> Level2Enemy1::sHurtRightFrames, Level2Enemy1::sHurtLeftFrames;
vector<sf::IntRect> Level2Enemy1::sIdleRightFrames, Level2Enemy1::sIdleLeftFrames;
vector<sf::IntRect> Level2Enemy1::sJumpRightFrames, Level2Enemy1::sJumpLeftFrames;
vector<sf::IntRect> Level2Enemy1::sRunRightFrames, Level2Enemy1::sRunLeftFrames;
vector<sf::IntRect> Level2Enemy1::sWalkRightFrames, Level2Enemy1::sWalkLeftFrames;

bool Level2Enemy1::sTexturesLoaded = false;

// =========================================================
// CONSTRUCTOR
// =========================================================
Level2Enemy1::Level2Enemy1(float startX, float startY) {
    // cout << "--- SPAWNING LEVEL 2 ENEMY AT (" << startX << ", " << startY << ") ---" << endl;

    mWorldPosition = sf::Vector2f(startX, startY);
    mHitboxWidth = 80.0f;
    mHitboxHeight = 64.0f;

    // Patrol Settings
    float patrolRadius = 200.0f; // Wider patrol
    mPatrolMinX = startX - patrolRadius;
    mPatrolMaxX = startX + patrolRadius;

    // Physics & Offsets
    mSpriteScale = 2.5f;
    mGroundOffset = 100.0f;
    mPlatformOffset = 85.0f;

    // Initialize AI Timers
    mJumpCooldown = 0.0f;
    mAttackCooldown = 0.0f;
    mSearchTimer = 0.0f;

    // Health (Stronger)
    mHealth = 80;
    mMaxHealth = 80;
    mPatrolPauseTimer = 0.0f;
    mLastKnownPlayerPos = mWorldPosition;

    // Load Textures (ONLY ONCE)
    if (!sTexturesLoaded) {
        loadAndSlice(sIdleRightSheet, "idleright.png", sIdleRightFrames, 8);
        loadAndSlice(sIdleLeftSheet, "idleleft.png", sIdleLeftFrames, 8);
        loadAndSlice(sWalkRightSheet, "walkright.png", sWalkRightFrames, 12);
        loadAndSlice(sWalkLeftSheet, "walkleft.png", sWalkLeftFrames, 12);
        loadAndSlice(sRunRightSheet, "runright.png", sRunRightFrames, 8);
        loadAndSlice(sRunLeftSheet, "runleft.png", sRunLeftFrames, 8);
        loadAndSlice(sJumpRightSheet, "jumpright.png", sJumpRightFrames, 3);
        loadAndSlice(sJumpLeftSheet, "jumpleft.png", sJumpLeftFrames, 3);
        loadAndSlice(sAttackRightSheet, "attack1right.png", sAttackRightFrames, 8);
        loadAndSlice(sAttackLeftSheet, "attack1left.png", sAttackLeftFrames, 8);
        loadAndSlice(sHurtRightSheet, "hurtright.png", sHurtRightFrames, 4);
        loadAndSlice(sHurtLeftSheet, "hurtleft.png", sHurtLeftFrames, 4);
        
        sTexturesLoaded = true;
    }

    if (!sIdleRightFrames.empty()) {
        mSprite.setTexture(sIdleRightSheet);
        mSprite.setTextureRect(sIdleRightFrames[0]);
    }
    mSprite.setScale(mSpriteScale, mSpriteScale);
    
    // TINT RED TO SHOW IT IS LEVEL 2
    mSprite.setColor(sf::Color::Red);

    mIsFacingRight = true;
    mState = EnemyState::PATROL;
}

Level2Enemy1::~Level2Enemy1() {}

void Level2Enemy1::loadAndSlice(sf::Texture& texture, string filename, vector<sf::IntRect>& frameVector, int frameCount) {
    // Reusing Level 1 assets
    string path = "assets/enemy/Level1Enemy1/" + filename;
    if (!texture.loadFromFile(path)) {
        if (!texture.loadFromFile("../" + path)) {
            texture.loadFromFile("C:/Users/Samiul/source/repos/FinalProject/FinalProject/" + path);
        }
    }
    int frameWidth = 80; int frameHeight = 64;
    frameVector.clear();
    for (int i = 0; i < frameCount; i++) {
        frameVector.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }
}

// =========================================================
// UPDATE LOGIC
// =========================================================
void Level2Enemy1::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    Enemy::update(deltaTime, playerPos, platforms);

    if (mJumpCooldown > 0.0f) mJumpCooldown -= deltaTime.asSeconds();
    if (mAttackCooldown > 0.0f) mAttackCooldown -= deltaTime.asSeconds();

    if (mState == EnemyState::DEAD) return;
    if (mState == EnemyState::HURT || mState == EnemyState::ATTACK) {
        updateAnimation(deltaTime);
        return;
    }

    // FASTER MOVEMENT FOR LEVEL 2
    float chaseSpeed = 250.0f; // Faster than Level 1

    // =========================================================
    // 1. INTELLIGENCE: TARGET SELECTION
    // =========================================================
    sf::Vector2f targetPos = playerPos;
    float distToPlayer = abs(mWorldPosition.x - playerPos.x);

    // 2. STATE MACHINE
    if (distToPlayer < 600.0f) {
        mState = EnemyState::CHASE;
        mLastKnownPlayerPos = playerPos;
    } else {
        mState = EnemyState::PATROL;
    }

    // 3. MOVEMENT EXECUTION
    if (mState == EnemyState::CHASE) {
        if (mWorldPosition.x < targetPos.x - 50.0f) {
            mVelocity.x = chaseSpeed;
            mIsFacingRight = true;
        } else if (mWorldPosition.x > targetPos.x + 50.0f) {
            mVelocity.x = -chaseSpeed;
            mIsFacingRight = false;
        } else {
            mVelocity.x = 0.0f;
            // Attack logic would go here
        }
    } 
    else if (mState == EnemyState::PATROL) {
        if (mIsFacingRight) {
            mVelocity.x = 100.0f;
            if (mWorldPosition.x > mPatrolMaxX) mIsFacingRight = false;
        } else {
            mVelocity.x = -100.0f;
            if (mWorldPosition.x < mPatrolMinX) mIsFacingRight = true;
        }
    }

    updateAnimation(deltaTime);
}

void Level2Enemy1::updateAnimation(sf::Time deltaTime) {
    mElapsedTime += deltaTime.asSeconds();
    if (mElapsedTime >= 0.1f) {
        mElapsedTime = 0.0f;
        mCurrentFrame++;

        vector<sf::IntRect>* currentAnim = nullptr;
        sf::Texture* currentSheet = nullptr;

        if (mState == EnemyState::IDLE) {
            currentAnim = mIsFacingRight ? &sIdleRightFrames : &sIdleLeftFrames;
            currentSheet = mIsFacingRight ? &sIdleRightSheet : &sIdleLeftSheet;
        } else if (mState == EnemyState::PATROL || mState == EnemyState::CHASE) {
            currentAnim = mIsFacingRight ? &sRunRightFrames : &sRunLeftFrames;
            currentSheet = mIsFacingRight ? &sRunRightSheet : &sRunLeftSheet;
        } else if (mState == EnemyState::ATTACK) {
            currentAnim = mIsFacingRight ? &sAttackRightFrames : &sAttackLeftFrames;
            currentSheet = mIsFacingRight ? &sAttackRightSheet : &sAttackLeftSheet;
        } else if (mState == EnemyState::HURT) {
            currentAnim = mIsFacingRight ? &sHurtRightFrames : &sHurtLeftFrames;
            currentSheet = mIsFacingRight ? &sHurtRightSheet : &sHurtLeftSheet;
        }

        if (currentAnim && !currentAnim->empty()) {
            if (mCurrentFrame >= currentAnim->size()) {
                mCurrentFrame = 0;
                
                if (mState == EnemyState::HURT) {
                    if (mHealth <= 0) {
                        mState = EnemyState::DEAD;
                    } else {
                        mState = EnemyState::PATROL;
                    }
                } else if (mState == EnemyState::ATTACK) {
                    mState = EnemyState::PATROL;
                }
            }
            mSprite.setTexture(*currentSheet);
            mSprite.setTextureRect((*currentAnim)[mCurrentFrame]);
        }
    }
}