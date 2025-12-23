#include "Level1Enemy1.h"
#include <iostream>
#include <cstdlib> 

using namespace std;

// =========================================================
// CONSTRUCTOR
// =========================================================
Level1Enemy1::Level1Enemy1(float startX, float startY) {
    cout << "--- SPAWNING INTELLIGENT ENEMY AT (" << startX << ", " << startY << ") ---" << endl;

    mWorldPosition = sf::Vector2f(startX, startY);
    mHitboxWidth = 80.0f;
    mHitboxHeight = 64.0f;

    // Patrol Settings
    float patrolRadius = 150.0f;
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
    mPatrolPauseTimer = 0.0f;
    mLastKnownPlayerPos = mWorldPosition;

    // Load Textures
    loadAndSlice(mIdleRightSheet, "idleright.png", mIdleRightFrames, 8);
    loadAndSlice(mIdleLeftSheet, "idleleft.png", mIdleLeftFrames, 8);
    loadAndSlice(mWalkRightSheet, "walkright.png", mWalkRightFrames, 12);
    loadAndSlice(mWalkLeftSheet, "walkleft.png", mWalkLeftFrames, 12);
    loadAndSlice(mRunRightSheet, "runright.png", mRunRightFrames, 8);
    loadAndSlice(mRunLeftSheet, "runleft.png", mRunLeftFrames, 8);
    loadAndSlice(mJumpRightSheet, "jumpright.png", mJumpRightFrames, 3);
    loadAndSlice(mJumpLeftSheet, "jumpleft.png", mJumpLeftFrames, 3);
    loadAndSlice(mAttackRightSheet, "attack1right.png", mAttackRightFrames, 8);
    loadAndSlice(mAttackLeftSheet, "attack1left.png", mAttackLeftFrames, 8);
    loadAndSlice(mHurtRightSheet, "hurtright.png", mHurtRightFrames, 4);
    loadAndSlice(mHurtLeftSheet, "hurtleft.png", mHurtLeftFrames, 4);

    if (!mIdleRightFrames.empty()) {
        mSprite.setTexture(mIdleRightSheet);
        mSprite.setTextureRect(mIdleRightFrames[0]);
    }
    mSprite.setScale(mSpriteScale, mSpriteScale);

    mIsFacingRight = true;
    mState = EnemyState::PATROL;
}

Level1Enemy1::~Level1Enemy1() {}

void Level1Enemy1::loadAndSlice(sf::Texture& texture, string filename, vector<sf::IntRect>& frameVector, int frameCount) {
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
void Level1Enemy1::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    Enemy::update(deltaTime, playerPos, platforms);

    if (mJumpCooldown > 0.0f) mJumpCooldown -= deltaTime.asSeconds();
    if (mAttackCooldown > 0.0f) mAttackCooldown -= deltaTime.asSeconds();

    if (mState == EnemyState::DEAD) return;
    if (mState == EnemyState::HURT || mState == EnemyState::ATTACK) {
        updateAnimation(deltaTime);
        return;
    }

    // =========================================================
    // 1. INTELLIGENCE: TARGET SELECTION
    // =========================================================
    sf::Vector2f targetPos = playerPos;

    // Check if player is High Up (Top Plat) AND Enemy is Low (Ground)
    bool playerOnTop = (playerPos.y < 400.0f);
    bool enemyOnGround = (mWorldPosition.y > 600.0f);

    if (playerOnTop && enemyOnGround) {
        // Player is unreachable directly. Find nearest Mid-Platform.
        float bestDist = 10000.0f;
        bool foundMid = false;
        sf::FloatRect midRect;

        for (const auto& plat : platforms) {
            sf::FloatRect bounds = plat.getGlobalBounds();
            // Look for platforms at Mid-Height (Y ~ 500)
            if (bounds.top > 450.0f && bounds.top < 550.0f) {
                float dist = abs((bounds.left + bounds.width / 2) - mWorldPosition.x);
                if (dist < bestDist) {
                    bestDist = dist;
                    midRect = bounds;
                    foundMid = true;
                }
            }
        }

        if (foundMid) {
            // Target the center of that platform
            targetPos = sf::Vector2f(midRect.left + midRect.width / 2.0f, midRect.top);
        }
    }

    // =========================================================
    // 2. MOVEMENT & BEHAVIOR
    // =========================================================
    float distToTargetX = targetPos.x - mWorldPosition.x;
    float absDistX = abs(distToTargetX);
    float distToTargetY = targetPos.y - mWorldPosition.y;

    // Real Player Distance (For Attack)
    float realDistToPlayer = abs(playerPos.x - mWorldPosition.x);
    float realDistY = abs(playerPos.y - mWorldPosition.y);

    // --- PRIORITY 1: ATTACK ---
    // STRICT FIX: Only attack if vertical distance is < 70.0f.
    // If you are below (dist > 100), this will be FALSE, so it won't freeze.
    if (realDistToPlayer < 100.0f && realDistY < 70.0f) {

        if (mAttackCooldown <= 0.0f) {
            mState = EnemyState::ATTACK;
            mVelocity.x = 0;
            mCurrentFrame = 0;
            mIsFacingRight = (playerPos.x > mWorldPosition.x);
            return;
        }
        else {
            // Cooldown: Just stand still
            mState = EnemyState::IDLE;
            mVelocity.x = 0;
            updateAnimation(deltaTime);
            return;
        }
    }

    // --- PRIORITY 2: CHASE ---
    else if (realDistToPlayer < 1200.0f) {
        mState = EnemyState::CHASE;

        // Is the Target (Player or Platform) significantly higher?
        bool needsToClimb = (distToTargetY < -100.0f);

        // Is the Target significantly lower?
        bool needsToDescend = (distToTargetY > 100.0f);

        // --- SCENARIO A: SAME LEVEL OR DESCENDING ---
        if (!needsToClimb) {
            // If descending (Player is below), we just run towards them.
            // Gravity will make us fall off the ledge naturally.

            // Fix: If we are directly above them (X ~ 0), ensure we keep moving to fall off.
            if (needsToDescend && absDistX < 20.0f) {
                // Force movement Right to walk off ledge
                mIsFacingRight = true;
                mVelocity.x = 220.0f;
            }
            else {
                // Normal Chase
                if (distToTargetX > 0) { mIsFacingRight = true; mVelocity.x = 220.0f; }
                else { mIsFacingRight = false; mVelocity.x = -220.0f; }
            }
        }

        // --- SCENARIO B: CLIMBING (STRICT ZONES) ---
        else {
            float minLaunch = 200.0f;
            float maxLaunch = 260.0f;

            // ZONE 1: STUCK UNDERNEATH (Too Close) -> RETREAT
            if (absDistX < minLaunch) {
                if (distToTargetX > 0) { mIsFacingRight = false; mVelocity.x = -220.0f; }
                else { mIsFacingRight = true; mVelocity.x = 220.0f; }
            }
            // ZONE 2: SWEET SPOT -> JUMP
            else if (absDistX >= minLaunch && absDistX <= maxLaunch) {
                if (distToTargetX > 0) { mIsFacingRight = true; mVelocity.x = 250.0f; }
                else { mIsFacingRight = false; mVelocity.x = -250.0f; }

                if (mIsOnGround && mJumpCooldown <= 0.0f) {
                    mVelocity.y = -950.0f;
                    mIsOnGround = false;
                    mJumpCooldown = 0.5f;
                }
            }
            // ZONE 3: TOO FAR -> RUN CLOSER
            else {
                if (distToTargetX > 0) { mIsFacingRight = true; mVelocity.x = 220.0f; }
                else { mIsFacingRight = false; mVelocity.x = -220.0f; }
            }
        }
    }
    // --- PRIORITY 3: PATROL ---
    else {
        mState = EnemyState::PATROL;
        if (mPatrolPauseTimer > 0.0f) {
            mVelocity.x = 0;
            mPatrolPauseTimer -= deltaTime.asSeconds();
        }
        else {
            if (mWorldPosition.x <= mPatrolMinX) {
                mIsFacingRight = true;
                if ((rand() % 100) < 30) mPatrolPauseTimer = 1.5f;
            }
            else if (mWorldPosition.x >= mPatrolMaxX) {
                mIsFacingRight = false;
                if ((rand() % 100) < 30) mPatrolPauseTimer = 1.5f;
            }
            if (mIsFacingRight) mVelocity.x = 100.0f;
            else mVelocity.x = -100.0f;
        }
    }

    updateAnimation(deltaTime);
}

// =========================================================
// ANIMATION HANDLER
// =========================================================
void Level1Enemy1::updateAnimation(sf::Time deltaTime) {
    mElapsedTime += deltaTime.asSeconds();
    if (mElapsedTime > 0.1f) {
        mElapsedTime = 0.0f;
        mCurrentFrame++;

        vector<sf::IntRect>* currentAnimFrames = &mIdleRightFrames;
        sf::Texture* currentTexture = &mIdleRightSheet;

        switch (mState) {
        case EnemyState::PATROL:
            if (mVelocity.x == 0) {
                if (mIsFacingRight) { currentAnimFrames = &mIdleRightFrames; currentTexture = &mIdleRightSheet; }
                else { currentAnimFrames = &mIdleLeftFrames; currentTexture = &mIdleLeftSheet; }
            }
            else {
                if (mIsFacingRight) { currentAnimFrames = &mWalkRightFrames; currentTexture = &mWalkRightSheet; }
                else { currentAnimFrames = &mWalkLeftFrames; currentTexture = &mWalkLeftSheet; }
            }
            break;

        case EnemyState::CHASE:
        case EnemyState::SEARCH:
            if (!mIsOnGround) {
                if (mIsFacingRight) { currentAnimFrames = &mJumpRightFrames; currentTexture = &mJumpRightSheet; }
                else { currentAnimFrames = &mJumpLeftFrames; currentTexture = &mJumpLeftSheet; }
            }
            else {
                if (mIsFacingRight) { currentAnimFrames = &mRunRightFrames; currentTexture = &mRunRightSheet; }
                else { currentAnimFrames = &mRunLeftFrames; currentTexture = &mRunLeftSheet; }
            }
            break;

        case EnemyState::ATTACK:
            if (mIsFacingRight) { currentAnimFrames = &mAttackRightFrames; currentTexture = &mAttackRightSheet; }
            else { currentAnimFrames = &mAttackLeftFrames; currentTexture = &mAttackLeftSheet; }
            break;

        case EnemyState::HURT:
            if (mIsFacingRight) { currentAnimFrames = &mHurtRightFrames; currentTexture = &mHurtRightSheet; }
            else { currentAnimFrames = &mHurtLeftFrames; currentTexture = &mHurtLeftSheet; }
            break;

        default:
            if (mIsFacingRight) { currentAnimFrames = &mIdleRightFrames; currentTexture = &mIdleRightSheet; }
            else { currentAnimFrames = &mIdleLeftFrames; currentTexture = &mIdleLeftSheet; }
            break;
        }

        if (currentAnimFrames->empty()) return;

        if (mCurrentFrame >= currentAnimFrames->size()) {
            mCurrentFrame = 0;
            if (mState == EnemyState::ATTACK || mState == EnemyState::HURT) {
                mState = EnemyState::PATROL;
                mAttackCooldown = 1.0f;
            }
        }

        mSprite.setTexture(*currentTexture);
        mSprite.setTextureRect((*currentAnimFrames)[mCurrentFrame]);
        mSprite.setScale(mSpriteScale, mSpriteScale);
        mSprite.setOrigin(0.0f, 0.0f);
    }
}