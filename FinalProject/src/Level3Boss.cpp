#include "Level3Boss.h"
#include <iostream>
#include <cstdlib> 
#include <cmath> 
#include <fstream>

using namespace std;

// Helper
bool fileExistsL3(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

// =========================================================
// STATIC MEMBER DEFINITIONS
// =========================================================
sf::Texture Level3Boss::sAttackSheet;
sf::Texture Level3Boss::sDeadSheet, Level3Boss::sHurtSheet, Level3Boss::sIdleSheet;
sf::Texture Level3Boss::sWalkSheet;

vector<sf::IntRect> Level3Boss::sAttackFrames;
vector<sf::IntRect> Level3Boss::sDeadFrames, Level3Boss::sHurtFrames, Level3Boss::sIdleFrames;
vector<sf::IntRect> Level3Boss::sWalkFrames;

bool Level3Boss::sTexturesLoaded = false;

// =========================================================
// CONSTRUCTOR
// =========================================================
Level3Boss::Level3Boss(float startX, float startY, float minX, float maxX) {
    mWorldPosition = sf::Vector2f(startX, startY);
    mHitboxWidth = 60.0f; 
    mHitboxHeight = 100.0f;

    // Patrol Settings
    mPatrolMinX = minX;
    mPatrolMaxX = maxX;

    // Physics & Offsets
    mSpriteScale = 1.5f; // Scaled up to 1.5x
    mGroundOffset = 64.0f * mSpriteScale; 
    mPlatformOffset = 64.0f * mSpriteScale;

    // Initialize AI Timers
    mAttackCooldown = 0.0f;

    // Health
    mHealth = 300; // Boss Health
    mMaxHealth = 300;
    mPatrolPauseTimer = 0.0f;

    // Load Textures (ONLY ONCE)
    if (!sTexturesLoaded) {
        std::cout << "--- LOADING LEVEL 3 BOSS TEXTURES ---" << std::endl;
        loadAndSlice(sAttackSheet, "Attack.png", sAttackFrames, 5);
        loadAndSlice(sDeadSheet, "Dead.png", sDeadFrames, 5);
        loadAndSlice(sHurtSheet, "Hurt.png", sHurtFrames, 3);
        loadAndSlice(sIdleSheet, "Idle.png", sIdleFrames, 10);
        loadAndSlice(sWalkSheet, "Walk.png", sWalkFrames, 12);
        sTexturesLoaded = true;
    } else {
        std::cout << "--- LEVEL 3 BOSS TEXTURES ALREADY LOADED ---" << std::endl;
    }

    // Initial State
    mState = IDLE;
    mSprite.setTexture(sIdleSheet);
    mSprite.setTextureRect(sIdleFrames[0]);
    mSprite.setOrigin(64.0f, 64.0f); // Center Origin (128x128 / 2)
    mSprite.setScale(mSpriteScale, mSpriteScale);
    mSprite.setPosition(mWorldPosition);

    mCurrentFrame = 0;
    mFrameTime = 0.0f;
    mAnimationSpeed = 0.1f;
    mIsFacingRight = true;
    
    // Initialize Health Bar
    mHealthBarBack.setSize(sf::Vector2f(50.0f, 5.0f));
    mHealthBarBack.setFillColor(sf::Color::Red);
    mHealthBarFront.setSize(sf::Vector2f(50.0f, 5.0f));
    mHealthBarFront.setFillColor(sf::Color::Green);
}

Level3Boss::~Level3Boss() {}

// =========================================================
// ASSET LOADING
// =========================================================
void Level3Boss::loadAndSlice(sf::Texture& texture, const std::string& filename, std::vector<sf::IntRect>& frames, int frameCount) {
    std::string path1 = "assets/enemy/Level3EnemyBoss/" + filename;
    std::string path2 = "../assets/enemy/Level3EnemyBoss/" + filename;
    std::string path3 = "FinalProject/assets/enemy/Level3EnemyBoss/" + filename;
    
    std::string finalPath = path1;
    if (fileExistsL3(path1)) finalPath = path1;
    else if (fileExistsL3(path2)) finalPath = path2;
    else if (fileExistsL3(path3)) finalPath = path3;

    if (!texture.loadFromFile(finalPath)) {
        std::cerr << "ERROR: Could not load " << finalPath << std::endl;
    } else {
        // Slice frames (128x128)
        for (int i = 0; i < frameCount; i++) {
            frames.push_back(sf::IntRect(i * 128, 0, 128, 128));
        }
    }
}

// =========================================================
// UPDATE LOOP
// =========================================================
void Level3Boss::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    // Debug Position
    // std::cout << "L3Boss Y: " << mWorldPosition.y << " VelY: " << mVelocity.y << std::endl;

    if (mState == DEAD) {
        updateAnimation(deltaTime);
        return;
    }

    // Apply Gravity
    mVelocity.y += 981.0f * deltaTime.asSeconds();
    mWorldPosition.y += mVelocity.y * deltaTime.asSeconds();

    // Ground Collision
    bool onGround = false;
    float feetOffset = 64.0f * mSpriteScale; // Distance from center to bottom of sprite (96.0f)

    for (const auto& platform : platforms) {
        sf::FloatRect pBounds = platform.getGlobalBounds();
        sf::FloatRect feet = getFeetHitbox();
        
        if (feet.intersects(pBounds) && mVelocity.y >= 0) {
            // Land on top
            // We want the feet (bottom of sprite) to be at pBounds.top
            // Feet Y = mWorldPosition.y + feetOffset - 10.0f
            // So: mWorldPosition.y + feetOffset - 10.0f = pBounds.top
            // mWorldPosition.y = pBounds.top - feetOffset + 10.0f
            
            if (mWorldPosition.y < pBounds.top + pBounds.height) {
                mWorldPosition.y = pBounds.top - feetOffset + 10.0f;
                mVelocity.y = 0;
                onGround = true;
            }
        }
    }

    // Update AI
    updateAI(deltaTime, playerPos);

    // Update Position X
    mWorldPosition.x += mVelocity.x * deltaTime.asSeconds();

    // Update Sprite
    mSprite.setPosition(mWorldPosition);
    
    // Face Player/Direction
    if (mVelocity.x > 0) mIsFacingRight = true;
    else if (mVelocity.x < 0) mIsFacingRight = false;
    else if (mState == ATTACK || mState == CHASE) {
        if (playerPos.x > mWorldPosition.x) mIsFacingRight = true;
        else mIsFacingRight = false;
    }

    if (mIsFacingRight) mSprite.setScale(mSpriteScale, mSpriteScale);
    else mSprite.setScale(-mSpriteScale, mSpriteScale);

    // Update Animation
    updateAnimation(deltaTime);

    // Update Health Bar
    float healthPercent = (float)mHealth / mMaxHealth;
    if (healthPercent < 0) healthPercent = 0;
    mHealthBarFront.setSize(sf::Vector2f(50.0f * healthPercent, 5.0f));
}

// =========================================================
// AI LOGIC
// =========================================================
void Level3Boss::updateAI(sf::Time deltaTime, sf::Vector2f playerPos) {
    float distToPlayer = std::abs(playerPos.x - mWorldPosition.x);
    float yDiff = std::abs(playerPos.y - mWorldPosition.y);

    // Cooldowns
    if (mAttackCooldown > 0) mAttackCooldown -= deltaTime.asSeconds();

    // State Machine
    if (mState == HURT) {
        // Wait for animation
        if (mCurrentFrame >= sHurtFrames.size() - 1) {
            mState = IDLE;
        }
        mVelocity.x = 0;
        return;
    }

    if (mState == ATTACK) {
        // Wait for animation
        if (mCurrentFrame >= sAttackFrames.size() - 1) {
            mState = IDLE;
            mAttackCooldown = 1.5f; // Cooldown between attacks
        }
        mVelocity.x = 0;
        return;
    }

    // Decisions
    if (distToPlayer < ATTACK_RANGE && yDiff < 100.0f && mAttackCooldown <= 0) {
        mState = ATTACK;
        mCurrentFrame = 0;
        mSprite.setTexture(sAttackSheet);
    }
    else if (distToPlayer < DETECTION_RANGE && yDiff < 200.0f) {
        // Chase
        mState = CHASE;
        if (playerPos.x > mWorldPosition.x) mVelocity.x = CHASE_SPEED;
        else mVelocity.x = -CHASE_SPEED;
    }
    else {
        // Patrol
        mState = PATROL;
        if (mPatrolPauseTimer > 0) {
            mPatrolPauseTimer -= deltaTime.asSeconds();
            mVelocity.x = 0;
            mState = IDLE;
        } else {
            if (mIsFacingRight) mVelocity.x = WALK_SPEED;
            else mVelocity.x = -WALK_SPEED;

            // Turn around at bounds
            if (mWorldPosition.x > mPatrolMaxX) {
                mWorldPosition.x = mPatrolMaxX;
                mIsFacingRight = false;
                mPatrolPauseTimer = 1.0f;
            } else if (mWorldPosition.x < mPatrolMinX) {
                mWorldPosition.x = mPatrolMinX;
                mIsFacingRight = true;
                mPatrolPauseTimer = 1.0f;
            }
        }
    }
}

// =========================================================
// ANIMATION
// =========================================================
void Level3Boss::updateAnimation(sf::Time deltaTime) {
    mFrameTime += deltaTime.asSeconds();
    if (mFrameTime >= mAnimationSpeed) {
        mFrameTime = 0;
        mCurrentFrame++;

        const vector<sf::IntRect>* currentFrames = nullptr;

        switch (mState) {
            case IDLE: 
                currentFrames = &sIdleFrames; 
                mSprite.setTexture(sIdleSheet);
                break;
            case PATROL: 
            case CHASE:
                currentFrames = &sWalkFrames; 
                mSprite.setTexture(sWalkSheet);
                break;
            case ATTACK: 
                currentFrames = &sAttackFrames; 
                mSprite.setTexture(sAttackSheet);
                break;
            case HURT: 
                currentFrames = &sHurtFrames; 
                mSprite.setTexture(sHurtSheet);
                break;
            case DEAD: 
                currentFrames = &sDeadFrames; 
                mSprite.setTexture(sDeadSheet);
                break;
        }

        if (currentFrames) {
            if (mCurrentFrame >= currentFrames->size()) {
                if (mState == DEAD) mCurrentFrame = currentFrames->size() - 1; // Stay dead
                else mCurrentFrame = 0; // Loop
            }
            mSprite.setTextureRect((*currentFrames)[mCurrentFrame]);
        }
    }
}

// =========================================================
// HITBOXES
// =========================================================
sf::FloatRect Level3Boss::getBodyHitbox() const {
    return sf::FloatRect(
        mWorldPosition.x - (mHitboxWidth / 2.0f), 
        mWorldPosition.y - (mHitboxHeight / 2.0f), 
        mHitboxWidth, 
        mHitboxHeight
    );
}

sf::FloatRect Level3Boss::getFeetHitbox() const {
    float feetOffset = 64.0f * mSpriteScale; // 96.0f
    return sf::FloatRect(
        mWorldPosition.x - (mHitboxWidth / 4.0f), 
        mWorldPosition.y + feetOffset - 10.0f, 
        mHitboxWidth / 2.0f, 
        10.0f
    );
}

void Level3Boss::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    // Debug Print every 60 frames or so (hacky but effective)
    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter % 600 == 0) {
        std::cout << "L3Boss Pos: (" << mWorldPosition.x << ", " << mWorldPosition.y << ") "
                  << "Screen Pos: (" << (mWorldPosition.x - cameraOffset.x) << ", " << (mWorldPosition.y - cameraOffset.y) << ") "
                  << "State: " << mState << std::endl;
    }

    // Draw Sprite
    mSprite.setPosition(mWorldPosition - cameraOffset);
    window.draw(mSprite);

    // Draw Health Bar (Centered above head)
    if (mState != DEAD) {
        float visualTop = (mWorldPosition.y - cameraOffset.y) - (64.0f * mSpriteScale);
        mHealthBarBack.setPosition((mWorldPosition.x - cameraOffset.x) - 25, visualTop - 15.0f);
        mHealthBarFront.setPosition((mWorldPosition.x - cameraOffset.x) - 25, visualTop - 15.0f);
        
        window.draw(mHealthBarBack);
        window.draw(mHealthBarFront);
    }
}
