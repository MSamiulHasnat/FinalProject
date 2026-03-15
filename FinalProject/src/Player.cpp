#include "Player.h"
#include <iostream>
#include <fstream> // Added for file check

using namespace std;

// Helper to check if file exists to avoid SFML error spam
bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

// Helper function to load textures
bool loadTexture(sf::Texture& texture, const string& folderName, const string& filename) {
    string paths[] = {
        "assets/player/" + folderName + "/" + filename,
        "FinalProject/assets/player/" + folderName + "/" + filename,
        "../assets/player/" + folderName + "/" + filename,
        "../../assets/player/" + folderName + "/" + filename,
        "../../../assets/player/" + folderName + "/" + filename
    };

    for (const auto& path : paths) {
        if (fileExists(path)) {
            if (texture.loadFromFile(path)) {
                // cout << "Loaded: " << path << endl; 
                return true;
            }
        }
    }
    
    cerr << "FAILED to load texture: " << filename << " from folder " << folderName << endl;
    return false;
}

Player::Player(int characterId)
    : mState(PlayerState::IDLE)
    , mDirection(Direction::RIGHT)
    , mWalkSpeed(200.0f)
    , mRunSpeed(400.0f)
    , mCurrentFrame(0)
    , mFrameTime(0.1f)
    , mElapsedTime(0.0f)
    , mWalkHoldTimer(0.0f)
    , mWorldPosition(0.0f, 0.0f)
    , mVelocity(0.0f, 0.0f)
    , mGravity(1800.0f)
    , mJumpSpeed(-800.0f)
    , mIsOnGround(false)
    , mCanSlide(true)
    , mHealth(100)
    , mMaxHealth(100)
    , mInvulnerabilityTimer(0.0f)
    , mHealingTimer(0.0f)
    , mCharacterId(characterId)
{
    string folderName = "Player" + to_string(characterId);

    if (mCharacterId == 1) {
        // 1. Load all 22 sprite sheets
        loadTexture(mIdleSheetRight, folderName, "idle1right.png");
        loadTexture(mIdleSheetLeft, folderName, "idle1left.png");
        loadTexture(mAttackSheetRight, folderName, "attack1right.png");
        loadTexture(mAttackSheetLeft, folderName, "attack1left.png");
        loadTexture(mWalkSheetRight, folderName, "walkright.png");
        loadTexture(mWalkSheetLeft, folderName, "walkleft.png");
        loadTexture(mRunSheetRight, folderName, "runright.png");
        loadTexture(mRunSheetLeft, folderName, "runleft.png");
        loadTexture(mJumpSheetRight, folderName, "jumpright.png");
        loadTexture(mJumpSheetLeft, folderName, "jumpleft.png");
        loadTexture(mFallSheetRight, folderName, "fallright.png");
        loadTexture(mFallSheetLeft, folderName, "fallleft.png");
        loadTexture(mSlideSheetRight, folderName, "slideright.png");
        loadTexture(mSlideSheetLeft, folderName, "slideleft.png");
        loadTexture(mWalkRollSheetRight, folderName, "walkrollright.png");
        loadTexture(mWalkRollSheetLeft, folderName, "walkrollleft.png");
        loadTexture(mRunRollSheetRight, folderName, "runrollright.png");
        loadTexture(mRunRollSheetLeft, folderName, "runrollleft.png");
        loadTexture(mAttack2SheetRight, folderName, "attack2right.png");
        loadTexture(mAttack2SheetLeft, folderName, "attack2left.png");
        loadTexture(mAttack3SheetRight, folderName, "attack3right.png");
        loadTexture(mAttack3SheetLeft, folderName, "attack3left.png");

        // Load Sitting Textures
        loadTexture(mSittingDownSheetRight, folderName, "sittingright.png");
        loadTexture(mSittingIdleSheetRight, folderName, "sitright.png");
        loadTexture(mStandingUpSheetRight, folderName, "standupright.png");

        // 2. === YOUR PARAMETERS ===
        int frameWidth = 96;
        int frameHeight = 64;
        int idleFrames = 8;
        int attackFrames = 7;
        int walkFrames = 10;
        int runFrames = 8;
        int jumpFrames = 6;
        int fallFrames = 4;
        int slideFrames = 9;
        int walkRollFrames = 8;
        int runRollFrames = 11;
        int attack2Frames = 8;
        int attack3Frames = 9;
        int sittingDownFrames = 6; // Assumed
        int sittingIdleFrames = 6; // Assumed
        int standingUpFrames = 6; // Assumed

        // 3. --- SLICE SPRITE SHEETS (WITH REVERSE LOGIC) ---
        sliceSpriteSheet(mIdleSheetRight, frameWidth, frameHeight, idleFrames, mIdleFramesRight, false);
        sliceSpriteSheet(mIdleSheetLeft, frameWidth, frameHeight, idleFrames, mIdleFramesLeft, true);
        sliceSpriteSheet(mAttackSheetRight, frameWidth, frameHeight, attackFrames, mAttackFramesRight, false);
        sliceSpriteSheet(mAttackSheetLeft, frameWidth, frameHeight, attackFrames, mAttackFramesLeft, true);
        sliceSpriteSheet(mWalkSheetRight, frameWidth, frameHeight, walkFrames, mWalkFramesRight, false);
        sliceSpriteSheet(mWalkSheetLeft, frameWidth, frameHeight, walkFrames, mWalkFramesLeft, true);
        sliceSpriteSheet(mRunSheetRight, frameWidth, frameHeight, runFrames, mRunFramesRight, false);
        sliceSpriteSheet(mRunSheetLeft, frameWidth, frameHeight, runFrames, mRunFramesLeft, true);
        sliceSpriteSheet(mJumpSheetRight, frameWidth, frameHeight, jumpFrames, mJumpFramesRight, false);
        sliceSpriteSheet(mJumpSheetLeft, frameWidth, frameHeight, jumpFrames, mJumpFramesLeft, true);
        sliceSpriteSheet(mFallSheetRight, frameWidth, frameHeight, fallFrames, mFallFramesRight, false);
        sliceSpriteSheet(mFallSheetLeft, frameWidth, frameHeight, fallFrames, mFallFramesLeft, true);
        sliceSpriteSheet(mSlideSheetRight, frameWidth, frameHeight, slideFrames, mSlideFramesRight, false);
        sliceSpriteSheet(mSlideSheetLeft, frameWidth, frameHeight, slideFrames, mSlideFramesLeft, true);
        sliceSpriteSheet(mWalkRollSheetRight, frameWidth, frameHeight, walkRollFrames, mWalkRollFramesRight, false);
        sliceSpriteSheet(mWalkRollSheetLeft, frameWidth, frameHeight, walkRollFrames, mWalkRollFramesLeft, true);
        sliceSpriteSheet(mRunRollSheetRight, frameWidth, frameHeight, runRollFrames, mRunRollFramesRight, false);
        sliceSpriteSheet(mRunRollSheetLeft, frameWidth, frameHeight, runRollFrames, mRunRollFramesLeft, true);
        sliceSpriteSheet(mAttack2SheetRight, frameWidth, frameHeight, attack2Frames, mAttack2FramesRight, false);
        sliceSpriteSheet(mAttack2SheetLeft, frameWidth, frameHeight, attack2Frames, mAttack2FramesLeft, true);
        sliceSpriteSheet(mAttack3SheetRight, frameWidth, frameHeight, attack3Frames, mAttack3FramesRight, false);
        sliceSpriteSheet(mAttack3SheetLeft, frameWidth, frameHeight, attack3Frames, mAttack3FramesLeft, true);

        sliceSpriteSheet(mSittingDownSheetRight, frameWidth, frameHeight, sittingDownFrames, mSittingDownFramesRight, false);
        sliceSpriteSheet(mSittingIdleSheetRight, frameWidth, frameHeight, sittingIdleFrames, mSittingIdleFramesRight, false);
        sliceSpriteSheet(mStandingUpSheetRight, frameWidth, frameHeight, standingUpFrames, mStandingUpFramesRight, false);
    }
    else if (mCharacterId == 2) {
        // Player 2 Logic
        // Load Textures (Assuming single sheet for both directions, will flip in render)
        loadTexture(mIdleSheetRight, folderName, "Idle.png");
        loadTexture(mIdleSheetLeft, folderName, "Idle.png"); // Same texture
        
        loadTexture(mWalkSheetRight, folderName, "Walk.png");
        loadTexture(mWalkSheetLeft, folderName, "Walk.png");
        
        loadTexture(mRunSheetRight, folderName, "Run.png");
        loadTexture(mRunSheetLeft, folderName, "Run.png");
        
        loadTexture(mJumpSheetRight, folderName, "Jump.png");
        loadTexture(mJumpSheetLeft, folderName, "Jump.png");
        
        loadTexture(mAttackSheetRight, folderName, "Attack_1.png");
        loadTexture(mAttackSheetLeft, folderName, "Attack_1.png");
        
        loadTexture(mAttack2SheetRight, folderName, "Attack_2.png");
        loadTexture(mAttack2SheetLeft, folderName, "Attack_2.png");
        
        loadTexture(mAttack3SheetRight, folderName, "Attack_3.png");
        loadTexture(mAttack3SheetLeft, folderName, "Attack_3.png");
        
        loadTexture(mDeadSheetRight, folderName, "Dead.png");
        loadTexture(mDeadSheetLeft, folderName, "Dead.png");
        
        loadTexture(mHurtSheetRight, folderName, "Hurt.png");
        loadTexture(mHurtSheetLeft, folderName, "Hurt.png");
        
        loadTexture(mProtectionSheetRight, folderName, "Protection.png");
        loadTexture(mProtectionSheetLeft, folderName, "Protection.png");

        // Parameters
        int frameWidth = 128;
        int frameHeight = 128;
        
        // Slice (Standard left-to-right for both, since we flip the sprite itself)
        sliceSpriteSheet(mIdleSheetRight, frameWidth, frameHeight, 6, mIdleFramesRight, false);
        sliceSpriteSheet(mIdleSheetLeft, frameWidth, frameHeight, 6, mIdleFramesLeft, false); // Note: false
        
        sliceSpriteSheet(mWalkSheetRight, frameWidth, frameHeight, 9, mWalkFramesRight, false);
        sliceSpriteSheet(mWalkSheetLeft, frameWidth, frameHeight, 9, mWalkFramesLeft, false);
        
        sliceSpriteSheet(mRunSheetRight, frameWidth, frameHeight, 8, mRunFramesRight, false);
        sliceSpriteSheet(mRunSheetLeft, frameWidth, frameHeight, 8, mRunFramesLeft, false);
        
        sliceSpriteSheet(mJumpSheetRight, frameWidth, frameHeight, 9, mJumpFramesRight, false);
        sliceSpriteSheet(mJumpSheetLeft, frameWidth, frameHeight, 9, mJumpFramesLeft, false);
        
        sliceSpriteSheet(mAttackSheetRight, frameWidth, frameHeight, 4, mAttackFramesRight, false);
        sliceSpriteSheet(mAttackSheetLeft, frameWidth, frameHeight, 4, mAttackFramesLeft, false);
        
        sliceSpriteSheet(mAttack2SheetRight, frameWidth, frameHeight, 5, mAttack2FramesRight, false);
        sliceSpriteSheet(mAttack2SheetLeft, frameWidth, frameHeight, 5, mAttack2FramesLeft, false);
        
        sliceSpriteSheet(mAttack3SheetRight, frameWidth, frameHeight, 4, mAttack3FramesRight, false);
        sliceSpriteSheet(mAttack3SheetLeft, frameWidth, frameHeight, 4, mAttack3FramesLeft, false);
        
        sliceSpriteSheet(mDeadSheetRight, frameWidth, frameHeight, 6, mDeadFramesRight, false);
        sliceSpriteSheet(mDeadSheetLeft, frameWidth, frameHeight, 6, mDeadFramesLeft, false);
        
        sliceSpriteSheet(mHurtSheetRight, frameWidth, frameHeight, 3, mHurtFramesRight, false);
        sliceSpriteSheet(mHurtSheetLeft, frameWidth, frameHeight, 3, mHurtFramesLeft, false);
        
        sliceSpriteSheet(mProtectionSheetRight, frameWidth, frameHeight, 2, mProtectionFramesRight, false);
        sliceSpriteSheet(mProtectionSheetLeft, frameWidth, frameHeight, 2, mProtectionFramesLeft, false);

        // Map missing states to existing frames
        mFallFramesRight = mJumpFramesRight; // Use Jump for Fall
        mFallFramesLeft = mJumpFramesLeft;
        
        mSlideFramesRight = mProtectionFramesRight; // Use Protection for Slide
        mSlideFramesLeft = mProtectionFramesLeft;
        
        mWalkRollFramesRight = mRunFramesRight; // Use Run for Roll
        mWalkRollFramesLeft = mRunFramesLeft;
        mRunRollFramesRight = mRunFramesRight;
        mRunRollFramesLeft = mRunFramesLeft;
        
        mSittingDownFramesRight = mIdleFramesRight; // Use Idle for Sit
        mSittingDownFramesLeft = mIdleFramesLeft;
        mSittingIdleFramesRight = mIdleFramesRight;
        mSittingIdleFramesLeft = mIdleFramesLeft;
        mStandingUpFramesRight = mIdleFramesRight;
        mStandingUpFramesLeft = mIdleFramesLeft;

        // --- FIX FOR WHITE BOX ---
        // We must ensure the texture variables for these mapped states are valid.
        // Since we are reusing frames, we should reuse the textures too.
        // loadTexture copies the image data, which is fine.
        loadTexture(mFallSheetRight, folderName, "Jump.png");
        loadTexture(mFallSheetLeft, folderName, "Jump.png");

        loadTexture(mSlideSheetRight, folderName, "Protection.png");
        loadTexture(mSlideSheetLeft, folderName, "Protection.png");

        loadTexture(mWalkRollSheetRight, folderName, "Run.png");
        loadTexture(mWalkRollSheetLeft, folderName, "Run.png");
        loadTexture(mRunRollSheetRight, folderName, "Run.png");
        loadTexture(mRunRollSheetLeft, folderName, "Run.png");

        loadTexture(mSittingDownSheetRight, folderName, "Idle.png");
        loadTexture(mSittingDownSheetLeft, folderName, "Idle.png");
        loadTexture(mSittingIdleSheetRight, folderName, "Idle.png");
        loadTexture(mSittingIdleSheetLeft, folderName, "Idle.png");
        loadTexture(mStandingUpSheetRight, folderName, "Idle.png");
        loadTexture(mStandingUpSheetLeft, folderName, "Idle.png");
    }

    // 4. Setup Sprite
    mSprite.setTexture(mIdleSheetRight);
    if (!mIdleFramesRight.empty()) {
        mSprite.setTextureRect(mIdleFramesRight[0]);
    }
    
    if (mCharacterId == 1) {
        mSprite.setScale(4.0f, 4.0f);
    } else {
        // Player 2 is 128x128. Player 1 was 96x64 scaled 4x -> 384x256.
        // To match Player 1 height (256), we scale Player 2 by 2x -> 256x256.
        mSprite.setScale(2.0f, 2.0f);
        // Origin is Top-Left (0,0) to match Player 1 logic
        mSprite.setOrigin(0.0f, 0.0f); 
    }
}

Player::~Player() {
    // Destructor
}

void Player::sliceSpriteSheet(const sf::Texture& texture, int frameWidth, int frameHeight, int frameCount, vector<sf::IntRect>& frames, bool isReversed) {
    if (!isReversed) {
        for (int i = 0; i < frameCount; i++) {
            frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }
    }
    else {
        for (int i = frameCount - 1; i >= 0; i--) {
            frames.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }
    }
}

void Player::handlePlayerInput(sf::Time deltaTime) {

    // =========================================================
    // 1. ATTACK INPUTS (Z, X, C) - CRITICAL FIX
    // =========================================================
    // We check these FIRST. If pressed, they trigger the attack and exit immediately.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        attack1();
        return; // Return so we don't accidentally overwrite the state with "Walk" below
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        attack2();
        return;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
        attack3();
        return;
    }

    // =========================================================
    // 2. BLOCKING STATES (Attacking, Rolling)
    // =========================================================
    // If we are currently attacking or rolling, we block all movement input.
    if (mState == PlayerState::ATTACK || mState == PlayerState::ATTACK2 || mState == PlayerState::ATTACK3 ||
        mState == PlayerState::WALK_ROLL || mState == PlayerState::RUN_ROLL) {

        // Polish: Allow walk timer to accumulate even during attack so you can run immediately after
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            mWalkHoldTimer += deltaTime.asSeconds();
        }
        else {
            mWalkHoldTimer = 0.0f;
        }

        // Apply friction
        if (mState == PlayerState::ATTACK || mState == PlayerState::ATTACK2 || mState == PlayerState::ATTACK3) {
            mVelocity.x *= 0.99f;
        }
        return; // STOP HERE. Do not process movement.
    }

    // =========================================================
    // 2.5. SITTING INPUT (E)
    // =========================================================
    // Start sitting
    if (mIsOnGround && (mState == PlayerState::IDLE || mState == PlayerState::WALK || mState == PlayerState::RUN)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
            mState = PlayerState::SITTING_DOWN;
            mCurrentFrame = 0;
            mVelocity.x = 0.0f;
            return;
        }
    }
    
    // While sitting or transitioning
    if (mState == PlayerState::SITTING_DOWN || mState == PlayerState::SITTING_IDLE) {
        mVelocity.x = 0.0f; // Ensure we don't slide
        
        // If we are in IDLE sit, check if E is released to stand up
        if (mState == PlayerState::SITTING_IDLE) {
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
                mState = PlayerState::STANDING_UP;
                mCurrentFrame = 0;
            }
        }
        return; // Block other inputs
    }

    // While standing up
    if (mState == PlayerState::STANDING_UP) {
        mVelocity.x = 0.0f;
        return; // Block other inputs
    }

    // =========================================================
    // 3. JUMP INPUT
    // =========================================================
    if (mIsOnGround && (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
        mVelocity.y = mJumpSpeed;
        mIsOnGround = false;
        mState = PlayerState::JUMP;
        mCurrentFrame = 0;
    }

    // =========================================================
    // 4. ROLL INPUT
    // =========================================================
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && mIsOnGround) {
        if (mState == PlayerState::RUN) {
            mState = PlayerState::RUN_ROLL;
            mCurrentFrame = 0;
            return;
        }
        if (mState == PlayerState::WALK) {
            mState = PlayerState::WALK_ROLL;
            mCurrentFrame = 0;
            return;
        }
    }

    // =========================================================
    // 5. MOVEMENT & SLIDE INPUT
    // =========================================================
    bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    bool down = sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

    if (mIsOnGround) {
        if (mState == PlayerState::SLIDE) {
            if (!down || !(left || right)) {
                mState = PlayerState::RUN;
                mWalkHoldTimer = mTimeToRun + 0.1f;
            }
        }
        else if (left) {
            mDirection = Direction::LEFT;
            mWalkHoldTimer += deltaTime.asSeconds();
            mState = (mWalkHoldTimer > mTimeToRun) ? PlayerState::RUN : PlayerState::WALK;
            if (mState == PlayerState::RUN && down && mCanSlide) {
                mState = PlayerState::SLIDE;
                mCurrentFrame = 0;
                mCanSlide = false;
            }
        }
        else if (right) {
            mDirection = Direction::RIGHT;
            mWalkHoldTimer += deltaTime.asSeconds();
            mState = (mWalkHoldTimer > mTimeToRun) ? PlayerState::RUN : PlayerState::WALK;
            if (mState == PlayerState::RUN && down && mCanSlide) {
                mState = PlayerState::SLIDE;
                mCurrentFrame = 0;
                mCanSlide = false;
            }
        }
        else {
            mWalkHoldTimer = 0.0f;
            mState = PlayerState::IDLE;
        }

        if (!down) {
            mCanSlide = true;
        }
    }
    // Air control
    else {
        if (left) mDirection = Direction::LEFT;
        if (right) mDirection = Direction::RIGHT;
        mCanSlide = true;
    }

    // =========================================================
    // 6. APPLY VELOCITY
    // =========================================================
    if (mState == PlayerState::SLIDE) {
        mVelocity.x *= 0.99f;
    }
    else if (left) {
        if (mState == PlayerState::RUN || mState == PlayerState::JUMP || mState == PlayerState::FALL)
            mVelocity.x = -mRunSpeed;
        else
            mVelocity.x = -mWalkSpeed;
    }
    else if (right) {
        if (mState == PlayerState::RUN || mState == PlayerState::JUMP || mState == PlayerState::FALL)
            mVelocity.x = mRunSpeed;
        else
            mVelocity.x = mWalkSpeed;
    }
    else {
        if (mState != PlayerState::WALK_ROLL && mState != PlayerState::RUN_ROLL) {
            mVelocity.x = 0.0f;
        }
    }
}


void Player::attack1() {
    if (mState != PlayerState::ATTACK && mState != PlayerState::ATTACK2 && mState != PlayerState::ATTACK3) {
        mState = PlayerState::ATTACK;
        mCurrentFrame = 0;
        mElapsedTime = 0.0f;
    }
}
void Player::attack2() {
    if (mState != PlayerState::ATTACK && mState != PlayerState::ATTACK2 && mState != PlayerState::ATTACK3) {
        mState = PlayerState::ATTACK2;
        mCurrentFrame = 0;
        mElapsedTime = 0.0f;
    }
}
void Player::attack3() {
    if (mState != PlayerState::ATTACK && mState != PlayerState::ATTACK2 && mState != PlayerState::ATTACK3) {
        mState = PlayerState::ATTACK3;
        mCurrentFrame = 0;
        mElapsedTime = 0.0f;
    }
}


// --- REMOVED getCollisionBounds() ---


// --- This is your new "FEET" box ---
sf::FloatRect Player::getFeetHitbox() const {
    // Unified Logic for both characters
    // Feet are at the bottom center of the visual sprite
    
    float visualWidth = getVisualWidth();
    float visualHeight = getVisualHeight();
    
    float hitboxWidth = 100.0f;
    float hitboxHeight = 15.0f;
    
    // Center X relative to Top-Left
    float centerX = mWorldPosition.x + (visualWidth / 2.0f);
    
    return sf::FloatRect(
        centerX - (hitboxWidth / 2.0f),
        mWorldPosition.y + visualHeight - hitboxHeight,
        hitboxWidth,
        hitboxHeight
    );
}

// --- This is your new "BODY" box ---
sf::FloatRect Player::getBodyHitbox() const {
    float visualWidth = getVisualWidth();
    float visualHeight = getVisualHeight();

    float hitboxWidth = 128.0f;
    
    float centerX = mWorldPosition.x + (visualWidth / 2.0f);

    return sf::FloatRect(
        centerX - (hitboxWidth / 2.0f),
        mWorldPosition.y, // Top
        hitboxWidth,
        visualHeight
    );
}

// --- THIS IS THE FIX ---
// This function now uses getFeetHitbox()
void Player::handleCollision(const sf::FloatRect& platformBounds) {

    // --- USE THE FEET HITBOX ---
    sf::FloatRect playerFeet = getFeetHitbox();

    // Check if the player's feet are intersecting a platform
    // AND the player is currently moving DOWN
    if (playerFeet.intersects(platformBounds) && mVelocity.y > 0) {

        // --- THIS IS A LANDING ---
        mIsOnGround = true;
        mVelocity.y = 0;

        float visualHeight = getVisualHeight();

        // Check if this is the main ground (Y=715)
        // Player 1 has empty space at bottom, so we sink them (+60).
        // Player 2 (256x256) has feet at bottom, so we sink them less or 0.
        // User requested Player 2 to be lower.
        float groundOffset = (mCharacterId == 2) ? 30.0f : 60.0f;
        float platformOffset = (mCharacterId == 2) ? 50.0f : 45.0f;

        if (platformBounds.top == (900.0f - 185.0f)) {
            mWorldPosition.y = (platformBounds.top - visualHeight) + groundOffset;
        }
        else {
            mWorldPosition.y = (platformBounds.top - visualHeight) + platformOffset;
        }
    }
    // NOTE: We no longer check for head-bonks or side-collisions
    // to prevent the vanish bug.
}
// --- END FIX ---


void Player::update(sf::Time deltaTime) {
    // Invulnerability Timer
    if (mInvulnerabilityTimer > 0.0f) {
        mInvulnerabilityTimer -= deltaTime.asSeconds();
        if (mInvulnerabilityTimer <= 0.0f) {
            mInvulnerabilityTimer = 0.0f;
            mSprite.setColor(sf::Color::White); // Reset color
        } else {
            // Flash effect
            int flash = static_cast<int>(mInvulnerabilityTimer * 10) % 2;
            mSprite.setColor(flash ? sf::Color::Red : sf::Color::White);
        }
    }

    // Healing Logic (Sitting Idle)
    if (mState == PlayerState::SITTING_IDLE) {
        mHealingTimer += deltaTime.asSeconds();
        if (mHealingTimer >= 2.0f) {
            mHealth += 1;
            if (mHealth > mMaxHealth) mHealth = mMaxHealth;
            mHealingTimer = 0.0f;
            // Optional: Visual feedback for healing could go here
        }
    } else {
        mHealingTimer = 0.0f;
    }

    // (This function is unchanged from your version)
    handlePlayerInput(deltaTime);

    mVelocity.y += mGravity * deltaTime.asSeconds();
    if (mVelocity.y > 1500) mVelocity.y = 1500;
    mWorldPosition += mVelocity * deltaTime.asSeconds();

    if (mState != PlayerState::ATTACK && mState != PlayerState::ATTACK2 && mState != PlayerState::ATTACK3 &&
        mState != PlayerState::WALK_ROLL && mState != PlayerState::RUN_ROLL && mState != PlayerState::SLIDE) {
        if (!mIsOnGround) {
            if (mState != PlayerState::JUMP && mVelocity.y < 0) {
                mState = PlayerState::JUMP;
                mCurrentFrame = 0;
            }
            else if (mState != PlayerState::FALL && mVelocity.y > 0) {
                mState = PlayerState::FALL;
                mCurrentFrame = 0;
            }
        }
        else {
            if (mState == PlayerState::JUMP || mState == PlayerState::FALL) {
                mState = PlayerState::IDLE;
                mCurrentFrame = 0;
            }
        }
    }

    mElapsedTime += deltaTime.asSeconds();
    if (mElapsedTime < mFrameTime) {
        return;
    }
    mElapsedTime = 0.0f;

    if (mState == PlayerState::ATTACK || mState == PlayerState::ATTACK2 || mState == PlayerState::ATTACK3 ||
        mState == PlayerState::WALK_ROLL || mState == PlayerState::RUN_ROLL ||
        mState == PlayerState::SLIDE || mState == PlayerState::JUMP ||
        mState == PlayerState::SITTING_DOWN || mState == PlayerState::STANDING_UP)
    {
        mCurrentFrame++;
        bool animationDone = false;
        size_t maxFrames = 0;
        if (mState == PlayerState::ATTACK) maxFrames = mAttackFramesRight.size();
        else if (mState == PlayerState::ATTACK2) maxFrames = mAttack2FramesRight.size();
        else if (mState == PlayerState::ATTACK3) maxFrames = mAttack3FramesRight.size();
        else if (mState == PlayerState::WALK_ROLL) maxFrames = mWalkRollFramesRight.size();
        else if (mState == PlayerState::RUN_ROLL) maxFrames = mRunRollFramesRight.size();
        else if (mState == PlayerState::SLIDE) maxFrames = mSlideFramesRight.size();
        else if (mState == PlayerState::SITTING_DOWN) maxFrames = mSittingDownFramesRight.size();
        else if (mState == PlayerState::STANDING_UP) maxFrames = mStandingUpFramesRight.size();
        else if (mState == PlayerState::DEAD) maxFrames = mDeadFramesRight.size();
        else if (mState == PlayerState::HURT) maxFrames = mHurtFramesRight.size();
        else if (mState == PlayerState::PROTECTION) maxFrames = mProtectionFramesRight.size();
        else if (mState == PlayerState::JUMP) {
            if (mCurrentFrame >= mJumpFramesRight.size()) {
                mCurrentFrame = mJumpFramesRight.size() - 1; // Hold on last frame
            }
        }
        if (mState != PlayerState::JUMP && mCurrentFrame >= maxFrames) {
            animationDone = true;
            // Don't reset frame for DEAD, we want to hold it
            if (mState == PlayerState::DEAD) {
                mCurrentFrame = maxFrames - 1;
                animationDone = false; // Keep "playing" the last frame
            } else {
                mCurrentFrame = 0;
            }
        }
        if (animationDone) {
            if (mState == PlayerState::SITTING_DOWN) {
                mState = PlayerState::SITTING_IDLE;
                mCurrentFrame = 0;
            }
            else if (mState == PlayerState::STANDING_UP) {
                mState = PlayerState::IDLE;
                mCurrentFrame = 0;
            }
            else if (mState == PlayerState::HURT || mState == PlayerState::PROTECTION) {
                mState = PlayerState::IDLE;
                mCurrentFrame = 0;
            }
            else if (!mIsOnGround) {
                mState = PlayerState::FALL;
            }
            else {
                bool moveHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
                if (moveHeld) {
                    mState = (mWalkHoldTimer > mTimeToRun) ? PlayerState::RUN : PlayerState::WALK;
                }
                else {
                    mState = PlayerState::IDLE;
                }
            }
        }
    }
    else
    {
        mCurrentFrame++;
        if (mState == PlayerState::IDLE && mCurrentFrame >= mIdleFramesRight.size()) mCurrentFrame = 0;
        else if (mState == PlayerState::WALK && mCurrentFrame >= mWalkFramesRight.size()) mCurrentFrame = 0;
        else if (mState == PlayerState::RUN && mCurrentFrame >= mRunFramesRight.size()) mCurrentFrame = 0;
        else if (mState == PlayerState::FALL && mCurrentFrame >= mFallFramesRight.size()) mCurrentFrame = 0;
        else if (mState == PlayerState::SITTING_IDLE && mCurrentFrame >= mSittingIdleFramesRight.size()) mCurrentFrame = 0;
    }

    // Set Sprite Texture Based on State
    switch (mState)
    {
    case PlayerState::IDLE:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mIdleSheetRight);
            if (!mIdleFramesRight.empty()) mSprite.setTextureRect(mIdleFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mIdleSheetLeft);
            if (!mIdleFramesLeft.empty()) mSprite.setTextureRect(mIdleFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::WALK:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mWalkSheetRight);
            if (!mWalkFramesRight.empty()) mSprite.setTextureRect(mWalkFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mWalkSheetLeft);
            if (!mWalkFramesLeft.empty()) mSprite.setTextureRect(mWalkFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::RUN:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mRunSheetRight);
            if (!mRunFramesRight.empty()) mSprite.setTextureRect(mRunFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mRunSheetLeft);
            if (!mRunFramesLeft.empty()) mSprite.setTextureRect(mRunFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::SLIDE:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mSlideSheetRight);
            if (!mSlideFramesRight.empty()) mSprite.setTextureRect(mSlideFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mSlideSheetLeft);
            if (!mSlideFramesLeft.empty()) mSprite.setTextureRect(mSlideFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::WALK_ROLL:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mWalkRollSheetRight);
            if (!mWalkRollFramesRight.empty()) mSprite.setTextureRect(mWalkRollFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mWalkRollSheetLeft);
            if (!mWalkRollFramesLeft.empty()) mSprite.setTextureRect(mWalkRollFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::RUN_ROLL:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mRunRollSheetRight);
            if (!mRunRollFramesRight.empty()) mSprite.setTextureRect(mRunRollFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mRunRollSheetLeft);
            if (!mRunRollFramesLeft.empty()) mSprite.setTextureRect(mRunRollFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::JUMP:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mJumpSheetRight);
            if (!mJumpFramesRight.empty()) mSprite.setTextureRect(mJumpFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mJumpSheetLeft);
            if (!mJumpFramesLeft.empty()) mSprite.setTextureRect(mJumpFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::FALL:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mFallSheetRight);
            if (!mFallFramesRight.empty()) mSprite.setTextureRect(mFallFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mFallSheetLeft);
            if (!mFallFramesLeft.empty()) mSprite.setTextureRect(mFallFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::ATTACK:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mAttackSheetRight);
            if (!mAttackFramesRight.empty()) mSprite.setTextureRect(mAttackFramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mAttackSheetLeft);
            if (!mAttackFramesLeft.empty()) mSprite.setTextureRect(mAttackFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::ATTACK2:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mAttack2SheetRight);
            if (!mAttack2FramesRight.empty()) mSprite.setTextureRect(mAttack2FramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mAttack2SheetLeft);
            if (!mAttack2FramesLeft.empty()) mSprite.setTextureRect(mAttack2FramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::ATTACK3:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mAttack3SheetRight);
            if (!mAttack3FramesRight.empty()) mSprite.setTextureRect(mAttack3FramesRight[mCurrentFrame]);
        }
        else {
            mSprite.setTexture(mAttack3SheetLeft);
            if (!mAttack3FramesLeft.empty()) mSprite.setTextureRect(mAttack3FramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::SITTING_DOWN:
        mSprite.setTexture(mSittingDownSheetRight);
        if (!mSittingDownFramesRight.empty()) mSprite.setTextureRect(mSittingDownFramesRight[mCurrentFrame]);
        break;
    case PlayerState::SITTING_IDLE:
        mSprite.setTexture(mSittingIdleSheetRight);
        if (!mSittingIdleFramesRight.empty()) mSprite.setTextureRect(mSittingIdleFramesRight[mCurrentFrame]);
        break;
    case PlayerState::STANDING_UP:
        mSprite.setTexture(mStandingUpSheetRight);
        if (!mStandingUpFramesRight.empty()) mSprite.setTextureRect(mStandingUpFramesRight[mCurrentFrame]);
        break;
    case PlayerState::DEAD:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mDeadSheetRight);
            if (!mDeadFramesRight.empty()) mSprite.setTextureRect(mDeadFramesRight[mCurrentFrame]);
        } else {
            mSprite.setTexture(mDeadSheetLeft);
            if (!mDeadFramesLeft.empty()) mSprite.setTextureRect(mDeadFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::HURT:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mHurtSheetRight);
            if (!mHurtFramesRight.empty()) mSprite.setTextureRect(mHurtFramesRight[mCurrentFrame]);
        } else {
            mSprite.setTexture(mHurtSheetLeft);
            if (!mHurtFramesLeft.empty()) mSprite.setTextureRect(mHurtFramesLeft[mCurrentFrame]);
        }
        break;
    case PlayerState::PROTECTION:
        if (mDirection == Direction::RIGHT) {
            mSprite.setTexture(mProtectionSheetRight);
            if (!mProtectionFramesRight.empty()) mSprite.setTextureRect(mProtectionFramesRight[mCurrentFrame]);
        } else {
            mSprite.setTexture(mProtectionSheetLeft);
            if (!mProtectionFramesLeft.empty()) mSprite.setTextureRect(mProtectionFramesLeft[mCurrentFrame]);
        }
        break;
    }

    // Player 2 Flipping Logic
    if (mCharacterId == 2) {
        if (mDirection == Direction::LEFT) {
            mSprite.setScale(-2.0f, 2.0f);
        } else {
            mSprite.setScale(2.0f, 2.0f);
        }
    }
}

void Player::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    sf::Vector2f drawPos = mWorldPosition - cameraOffset;
    
    // If Player 2 is facing Left, we need to offset the drawing position
    // because we are using Top-Left origin (0,0) with negative scale.
    // Negative scale with (0,0) origin draws to the LEFT of the position.
    // So we must shift it RIGHT by the visual width to draw it in the correct spot.
    if (mCharacterId == 2 && mDirection == Direction::LEFT) {
        drawPos.x += getVisualWidth();
    }

    mSprite.setPosition(drawPos);
    window.draw(mSprite);
}

void Player::takeDamage(int damage) {
    if (mInvulnerabilityTimer > 0.0f) return;

    mHealth -= damage;
    if (mHealth < 0) mHealth = 0;
    
    // 1 second invulnerability
    mInvulnerabilityTimer = 1.0f; 
    
    // Visual feedback (flash red)
    mSprite.setColor(sf::Color::Red);
}

bool Player::isAttacking() const {
    return mState == PlayerState::ATTACK || 
           mState == PlayerState::ATTACK2 || 
           mState == PlayerState::ATTACK3;
}

int Player::getAttackType() const {
    if (mState == PlayerState::ATTACK) return 1;
    if (mState == PlayerState::ATTACK2) return 2;
    if (mState == PlayerState::ATTACK3) return 3;
    return 0;
}

sf::FloatRect Player::getAttackHitbox() const {
    if (!isAttacking()) return sf::FloatRect(0,0,0,0);

    sf::FloatRect body = getBodyHitbox();
    float range = 50.0f; // Attack range
    
    if (mDirection == Direction::RIGHT) {
        return sf::FloatRect(body.left + body.width, body.top, range, body.height);
    } else {
        return sf::FloatRect(body.left - range, body.top, range, body.height);
    }
}