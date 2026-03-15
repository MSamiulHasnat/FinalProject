#include "Level1Boss.h"
#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

// Helper to check if file exists
bool fileExistsL1(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

// =========================================================
// STATIC MEMBER DEFINITIONS
// =========================================================
sf::Texture Level1Boss::sIdleRightSheet, Level1Boss::sIdleLeftSheet;
sf::Texture Level1Boss::sFlyingRightSheet, Level1Boss::sFlyingLeftSheet;
sf::Texture Level1Boss::sAttackRightSheet, Level1Boss::sAttackLeftSheet;
sf::Texture Level1Boss::sHurtRightSheet, Level1Boss::sHurtLeftSheet;
sf::Texture Level1Boss::sDeathRightSheet, Level1Boss::sDeathLeftSheet;

vector<sf::IntRect> Level1Boss::sIdleRightFrames, Level1Boss::sIdleLeftFrames;
vector<sf::IntRect> Level1Boss::sFlyingRightFrames, Level1Boss::sFlyingLeftFrames;
vector<sf::IntRect> Level1Boss::sAttackRightFrames, Level1Boss::sAttackLeftFrames;
vector<sf::IntRect> Level1Boss::sHurtRightFrames, Level1Boss::sHurtLeftFrames;
vector<sf::IntRect> Level1Boss::sDeathRightFrames, Level1Boss::sDeathLeftFrames;

bool Level1Boss::sTexturesLoaded = false;

Level1Boss::Level1Boss(float startX, float startY, bool isMiniBoss) {
    cout << "--- SPAWNING BOSS: FLYING DEMON AT (" << startX << ", " << startY << ") ---" << endl;

    mWorldPosition = sf::Vector2f(startX, startY);
    mSpawnPoint = mWorldPosition;
    mIsMiniBoss = isMiniBoss;
    
    if (isMiniBoss) {
        // Mini Boss Stats (Normal Enemy in Level 2)
        mMaxHealth = 80; 
        mHealth = mMaxHealth;
        mSpriteScale = 1.2f; // Smaller scale
        mHitboxWidth = 40.0f; 
        mHitboxHeight = 35.0f; 
        mChaseSpeed = 150.0f; // Slightly slower
    } else {
        // Boss Stats
        mMaxHealth = 300; 
        mHealth = mMaxHealth;
        mSpriteScale = 2.0f; // 2x Scale
        mHitboxWidth = 70.0f; 
        mHitboxHeight = 60.0f; 
        mChaseSpeed = 180.0f; 
    }
    
    // Physics
    mGravity = 0.0f; // FLYING ENEMY - No Gravity

    mAttackCooldown = 0.0f;
    mAttackRange = 150.0f; // Increased range for larger sprite

    // Load Textures (ONLY ONCE)
    if (!sTexturesLoaded) {
        // Helper lambda to load and flip
        auto loadFlipped = [&](sf::Texture& right, sf::Texture& left, string filename, vector<sf::IntRect>& rFrames, vector<sf::IntRect>& lFrames, int count) {
             string base = "assets/enemy/Level1EnemyBoss/" + filename;
             string paths[] = {
                 base,
                 "FinalProject/" + base,
                 "../" + base
             };

             sf::Image img;
             bool loaded = false;
             for (const auto& p : paths) {
                 if (fileExistsL1(p)) {
                     if (img.loadFromFile(p)) {
                         loaded = true;
                         break;
                     }
                 }
             }

             if (!loaded) {
                 cout << "Failed to load boss texture: " << base << endl;
                 return;
             }
             
             // Load Right Texture
             right.loadFromImage(img);
             
             // Flip Image for Left Texture
             img.flipHorizontally();
             left.loadFromImage(img);
             
             // Create Frames (Same for both)
             int frameWidth = 81; int frameHeight = 71;
             rFrames.clear();
             lFrames.clear();
             for (int i = 0; i < count; i++) {
                 sf::IntRect rect(i * frameWidth, 0, frameWidth, frameHeight);
                 rFrames.push_back(rect);
                 lFrames.push_back(rect);
             }
        };

        loadFlipped(sIdleRightSheet, sIdleLeftSheet, "IDLE.png", sIdleRightFrames, sIdleLeftFrames, 4);
        loadFlipped(sFlyingRightSheet, sFlyingLeftSheet, "FLYING.png", sFlyingRightFrames, sFlyingLeftFrames, 4);
        loadFlipped(sAttackRightSheet, sAttackLeftSheet, "ATTACK.png", sAttackRightFrames, sAttackLeftFrames, 8);
        loadFlipped(sHurtRightSheet, sHurtLeftSheet, "HURT.png", sHurtRightFrames, sHurtLeftFrames, 4);
        loadFlipped(sDeathRightSheet, sDeathLeftSheet, "DEATH.png", sDeathRightFrames, sDeathLeftFrames, 7);

        sTexturesLoaded = true;
    }

    if (!sIdleRightFrames.empty()) {
        mSprite.setTexture(sIdleRightSheet);
        mSprite.setTextureRect(sIdleRightFrames[0]);
    }
    mSprite.setScale(mSpriteScale, mSpriteScale);
    mState = EnemyState::IDLE;
}

Level1Boss::~Level1Boss() {}

void Level1Boss::update(sf::Time deltaTime, sf::Vector2f playerPos, const std::vector<sf::Sprite>& platforms) {
    
    // --- 1. COOLDOWNS ---
    if (mAttackCooldown > 0.0f) mAttackCooldown -= deltaTime.asSeconds();
    if (mDamageCooldown > 0.0f) mDamageCooldown -= deltaTime.asSeconds();

    // --- UPDATE HEALTH BAR ---
    if (mState != EnemyState::DEAD) {
        float healthPct = static_cast<float>(mHealth) / static_cast<float>(mMaxHealth);
        if (healthPct < 0) healthPct = 0;
        mHealthBarFront.setSize(sf::Vector2f(50.0f * healthPct, 6.0f));
    }

    // --- 2. STATE LOCKS ---
    if (mState == EnemyState::DEAD) {
        updateAnimation(deltaTime);
        return; // Don't move if dead
    }
    if (mState == EnemyState::HURT || mState == EnemyState::ATTACK) {
        updateAnimation(deltaTime);
        return; // Don't move while hurting or attacking
    }

    // --- 3. AI: FLYING CHASE ---
    float dx = playerPos.x - mWorldPosition.x;
    float dy = playerPos.y - mWorldPosition.y;
    float dist = std::sqrt(dx*dx + dy*dy);

    // Face Player
    if (dx > 0) mIsFacingRight = true;
    else mIsFacingRight = false;

    // ATTACK LOGIC
    if (dist < mAttackRange && mAttackCooldown <= 0.0f) {
        mState = EnemyState::ATTACK;
        mCurrentFrame = 0;
        mVelocity = sf::Vector2f(0, 0); // Stop to attack
        return;
    }

    // MOVEMENT LOGIC
    if (dist < 1000.0f) { // Only wake up if player is somewhat close
        mState = EnemyState::CHASE; // Use "Flying" animation state

        // Normalize direction
        float dirX = dx / dist;
        float dirY = dy / dist;

        mVelocity.x = dirX * mChaseSpeed;
        mVelocity.y = dirY * mChaseSpeed;

        // "Should not come down" Constraint:
        // Platform 7/8 is at Y=270. Boss must stay ABOVE it (Y < 270).
        // Boss Height is ~60 * 2 = 120.
        // Let's clamp Y to be <= 200.0f to be safe.
        if (mWorldPosition.y > 200.0f) {
             mWorldPosition.y = 200.0f;
             if (mVelocity.y > 0) mVelocity.y = 0; // Stop moving down
        }

        // --- GUARD PLATFORM 7 & 8 CONSTRAINT ---
        // Only apply strict guarding for the Main Boss
        if (!mIsMiniBoss) {
            // Platform 7 starts at 3800. Platform 8 ends at 3800 + 356 + 356 = 4512.
            float minX = 3800.0f;
            float maxX = 4512.0f;

            // Clamp X position
            if (mWorldPosition.x < minX) {
                mWorldPosition.x = minX;
                mVelocity.x = 0; // Stop moving left
            }
            else if (mWorldPosition.x > maxX) {
                mWorldPosition.x = maxX;
                mVelocity.x = 0; // Stop moving right
            }
        }
    } else {
        mState = EnemyState::IDLE;
        mVelocity = sf::Vector2f(0, 0);
    }

    // Apply Movement (No Gravity)
    mWorldPosition += mVelocity * deltaTime.asSeconds();
    
    updateAnimation(deltaTime);
}

void Level1Boss::updateAnimation(sf::Time deltaTime) {
    mElapsedTime += deltaTime.asSeconds();
    if (mElapsedTime > 0.1f) {
        mElapsedTime = 0.0f;
        mCurrentFrame++;
    }

    // Select Texture & Frames (OUTSIDE timer check to ensure immediate updates)
    vector<sf::IntRect>* currentFrames = &sIdleRightFrames;
    sf::Texture* currentTexture = &sIdleRightSheet;

    switch (mState) {
        case EnemyState::IDLE:
            if (mIsFacingRight) { currentFrames = &sIdleRightFrames; currentTexture = &sIdleRightSheet; }
            else { currentFrames = &sIdleLeftFrames; currentTexture = &sIdleLeftSheet; }
            break;
        case EnemyState::CHASE: // Flying
            if (mIsFacingRight) { currentFrames = &sFlyingRightFrames; currentTexture = &sFlyingRightSheet; }
            else { currentFrames = &sFlyingLeftFrames; currentTexture = &sFlyingLeftSheet; }
            break;
        case EnemyState::ATTACK:
            if (mIsFacingRight) { currentFrames = &sAttackRightFrames; currentTexture = &sAttackRightSheet; }
            else { currentFrames = &sAttackLeftFrames; currentTexture = &sAttackLeftSheet; }
            break;
        case EnemyState::HURT:
            if (mIsFacingRight) { currentFrames = &sHurtRightFrames; currentTexture = &sHurtRightSheet; }
            else { currentFrames = &sHurtLeftFrames; currentTexture = &sHurtLeftSheet; }
            break;
        case EnemyState::DEAD:
            if (mIsFacingRight) { currentFrames = &sDeathRightFrames; currentTexture = &sDeathRightSheet; }
            else { currentFrames = &sDeathLeftFrames; currentTexture = &sDeathLeftSheet; }
            break;
        default:
            if (mIsFacingRight) { currentFrames = &sIdleRightFrames; currentTexture = &sIdleRightSheet; }
            else { currentFrames = &sIdleLeftFrames; currentTexture = &sIdleLeftSheet; }
            break;
    }

    if (currentFrames->empty()) return;

    // Loop Logic
    if (mCurrentFrame >= currentFrames->size()) {
        if (mState == EnemyState::DEAD) {
            mCurrentFrame = currentFrames->size() - 1; // Stay dead on last frame
        }
        else {
            // Reset State after one-shot animations
            if (mState == EnemyState::ATTACK || mState == EnemyState::HURT) {
                
                // CHECK DEATH (If health is 0 after hurt animation, die)
                if (mState == EnemyState::HURT && mHealth <= 0) {
                    mState = EnemyState::DEAD;
                    mCurrentFrame = 0;
                    if (mIsFacingRight) { currentFrames = &sDeathRightFrames; currentTexture = &sDeathRightSheet; }
                    else { currentFrames = &sDeathLeftFrames; currentTexture = &sDeathLeftSheet; }
                }
                else {
                    mState = EnemyState::CHASE; // Go back to flying
                    if (mState == EnemyState::ATTACK) mAttackCooldown = 1.5f;
                    
                    // Reset frame for the NEW state
                    mCurrentFrame = 0;
                    
                    // Update pointers for the new state immediately to avoid glitch
                    if (mIsFacingRight) { currentFrames = &sFlyingRightFrames; currentTexture = &sFlyingRightSheet; }
                    else { currentFrames = &sFlyingLeftFrames; currentTexture = &sFlyingLeftSheet; }
                }
            }
            else {
                mCurrentFrame = 0;
            }
        }
    }

    // Apply Texture & Rect (Every frame)
    if (mCurrentFrame < currentFrames->size()) {
        mSprite.setTexture(*currentTexture);
        mSprite.setTextureRect((*currentFrames)[mCurrentFrame]);
        mSprite.setScale(mSpriteScale, mSpriteScale);
        mSprite.setOrigin(0, 0); 
    }
}

void Level1Boss::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    // Draw Sprite
    mSprite.setPosition(
        mWorldPosition.x - cameraOffset.x,
        mWorldPosition.y - cameraOffset.y
    );
    window.draw(mSprite);

    // Draw Health Bar (if not dead)
    if (mState != EnemyState::DEAD) {
        // Calculate Center of Sprite
        // Frame Width is 81.
        float visualWidth = 81.0f * mSpriteScale;
        
        // Center X
        float barX = (mWorldPosition.x - cameraOffset.x) + (visualWidth / 2.0f) - 25.0f;
        
        // Top Y (Just above the sprite)
        // Since Origin is (0,0), mWorldPosition.y is the top.
        // Put it 15px above.
        float barY = (mWorldPosition.y - cameraOffset.y) - 15.0f;

        mHealthBarBack.setPosition(barX, barY);
        mHealthBarFront.setPosition(barX, barY);

        window.draw(mHealthBarBack);
        window.draw(mHealthBarFront);
    }
}