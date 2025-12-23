#include "Player.h"
#include <iostream>

using namespace std;

// Helper function to load textures
bool loadTexture(sf::Texture& texture, const string& filename) {
    string path1 = "assets/player/Player1/" + filename;
    if (texture.loadFromFile(path1)) {
        cout << "Loaded: " << path1 << endl;
        return true;
    }
    string path2 = "../assets/player/Player1/" + filename;
    if (texture.loadFromFile(path2)) {
        cout << "Loaded: " << path2 << endl;
        return true;
    }
    string path3 = "C:/Users/Samiul/source/repos/FinalProject/FinalProject/assets/player/Player1/" + filename;
    if (texture.loadFromFile(path3)) {
        cout << "Loaded: " << path3 << endl;
        return true;
    }
    cerr << "--- CRITICAL FAILURE --- Could not load: " << filename << endl;
    return false;
}

Player::Player()
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
{
    // 1. Load all 22 sprite sheets
    loadTexture(mIdleSheetRight, "idle1right.png");
    loadTexture(mIdleSheetLeft, "idle1left.png");
    loadTexture(mAttackSheetRight, "attack1right.png");
    loadTexture(mAttackSheetLeft, "attack1left.png");
    loadTexture(mWalkSheetRight, "walkright.png");
    loadTexture(mWalkSheetLeft, "walkleft.png");
    loadTexture(mRunSheetRight, "runright.png");
    loadTexture(mRunSheetLeft, "runleft.png");
    loadTexture(mJumpSheetRight, "jumpright.png");
    loadTexture(mJumpSheetLeft, "jumpleft.png");
    loadTexture(mFallSheetRight, "fallright.png");
    loadTexture(mFallSheetLeft, "fallleft.png");
    loadTexture(mSlideSheetRight, "slideright.png");
    loadTexture(mSlideSheetLeft, "slideleft.png");
    loadTexture(mWalkRollSheetRight, "walkrollright.png");
    loadTexture(mWalkRollSheetLeft, "walkrollleft.png");
    loadTexture(mRunRollSheetRight, "runrollright.png");
    loadTexture(mRunRollSheetLeft, "runrollleft.png");
    loadTexture(mAttack2SheetRight, "attack2right.png");
    loadTexture(mAttack2SheetLeft, "attack2left.png");
    loadTexture(mAttack3SheetRight, "attack3right.png");
    loadTexture(mAttack3SheetLeft, "attack3left.png");

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

    // 4. Setup Sprite
    mSprite.setTexture(mIdleSheetRight);
    if (!mIdleFramesRight.empty()) {
        mSprite.setTextureRect(mIdleFramesRight[0]);
    }
    mSprite.setScale(4.0f, 4.0f);
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
    float fullWidth = 96.0f * 4.0f;
    float fullHeight = 64.0f * 4.0f;

    float hitboxWidth = 128.0f;
    float xOffset = (fullWidth - hitboxWidth) / 2.0f;

    float hitboxHeight = 10.0f;
    float yOffset = fullHeight - hitboxHeight;

    return sf::FloatRect(
        mWorldPosition.x + xOffset,
        mWorldPosition.y + yOffset,
        hitboxWidth,
        hitboxHeight
    );
}

// --- This is your new "BODY" box ---
sf::FloatRect Player::getBodyHitbox() const {
    float fullWidth = 96.0f * 4.0f;
    float fullHeight = 64.0f * 4.0f;

    float hitboxWidth = 128.0f;
    float xOffset = (fullWidth - hitboxWidth) / 2.0f;

    return sf::FloatRect(
        mWorldPosition.x + xOffset,
        mWorldPosition.y,
        hitboxWidth,
        fullHeight
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

        float playerHeight = 64.0f * 4.0f; // 256px

        // Check if this is the main ground (Y=715)
        if (platformBounds.top == (900.0f - 185.0f)) {
            // Ground: Snap 60px in
            mWorldPosition.y = (platformBounds.top - playerHeight) + 60.0f;
        }
        else {
            // Mid-air: Snap 45px in
            mWorldPosition.y = (platformBounds.top - playerHeight) + 45.0f;
        }
    }
    // NOTE: We no longer check for head-bonks or side-collisions
    // to prevent the vanish bug.
}
// --- END FIX ---


void Player::update(sf::Time deltaTime) {
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
        mState == PlayerState::SLIDE || mState == PlayerState::JUMP)
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
        else if (mState == PlayerState::JUMP) {
            if (mCurrentFrame >= mJumpFramesRight.size()) {
                mCurrentFrame = mJumpFramesRight.size() - 1; // Hold on last frame
            }
        }
        if (mState != PlayerState::JUMP && mCurrentFrame >= maxFrames) {
            animationDone = true;
            mCurrentFrame = 0;
        }
        if (animationDone) {
            if (!mIsOnGround) {
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
    }
}

void Player::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    mSprite.setPosition(
        mWorldPosition.x - cameraOffset.x,
        mWorldPosition.y - cameraOffset.y
    );
    window.draw(mSprite);
}