#include "Game.h"
#include "Level1Enemy1.h" 
#include "Level2Enemy1.h" 
#include "Level1Boss.h" // Required for the Boss
#include "Level2Boss.h"
#include "Level3Boss.h"
#include <iostream>
#include <cstdlib> 
#include <ctime>   

using namespace std;

// ==================================================
// CONSTRUCTOR
// ==================================================
Game::Game()
    : mWindow(sf::VideoMode(1600, 900), "Final Project: Time Survivor")
    , mIsRunning(true)
    , mState(STATE_MENU)
    , mCameraPosition(0, 0)
    , mLevelTimer(60.0f)
    , mScore(0)
    , mCurrentLevel(1)
    , mUnlockedCharacters(2) // DEBUG: Unlock Player 2
    , mSelectedCharacter(2)  // DEBUG: Select Player 2
    , mBoss(nullptr) // Initialize Boss pointer
    , mGateway(nullptr)
    , mParallaxBackground(nullptr)
{
    mWindow.setFramerateLimit(60);

    // Seed Randomness
    srand(static_cast<unsigned int>(time(NULL)));

    // --- AUDIO ---
    AudioManager::getInstance().playMusic("assets/audio/bgmusic.mp3");

    mMenu = new Menu(static_cast<float>(mWindow.getSize().x), static_cast<float>(mWindow.getSize().y));
    mPlayer = new Player(mSelectedCharacter);

    // --- BACKGROUND ---
    // Initial load, will be handled by loadLevel() mostly
    string level1_background_path = "assets/map/Level1.png";
    mBackground = new StaticBackground(
        static_cast<float>(mWindow.getSize().x),
        static_cast<float>(mWindow.getSize().y),
        level1_background_path
    );
    mLevelWidth = mBackground->getTextureWidth() * 3.0f;

    // --- TEXTURES ---
    string platformPath = "assets/map/Level1Land.png";
    if (!mPlatformTexture.loadFromFile(platformPath) &&
        !mPlatformTexture.loadFromFile("../" + platformPath) &&
        !mPlatformTexture.loadFromFile("../../" + platformPath) &&
        !mPlatformTexture.loadFromFile("../../../" + platformPath) &&
        !mPlatformTexture.loadFromFile("FinalProject/" + platformPath)) {
             cerr << "Failed to load platform texture!" << endl;
    }
    mPlatformTexture.setRepeated(true);

    // ==========================================
    // --- UI SETUP ---
    // ==========================================
    // 1. Load Font (Only "SupercellMagic.ttf")
    if (!mFont.loadFromFile("assets/fonts/SupercellMagic.ttf") && 
        !mFont.loadFromFile("../assets/fonts/SupercellMagic.ttf") &&
        !mFont.loadFromFile("../../assets/fonts/SupercellMagic.ttf") &&
        !mFont.loadFromFile("FinalProject/assets/fonts/SupercellMagic.ttf")) {
        
        // If failed, print error but do NOT load Arial
        std::cerr << "ERROR: Failed to load SupercellMagic.ttf" << std::endl;
    }

    // 2. UI Background (Made more subtle/invisible, relying on text outlines)
    mUiBackground.setSize(sf::Vector2f(0, 0)); // Hidden for cleaner look
    // If you prefer a box, uncomment below:
    // mUiBackground.setSize(sf::Vector2f(220.0f, 140.0f));
    // mUiBackground.setFillColor(sf::Color(0, 0, 0, 100));
    // mUiBackground.setPosition(10, 10);
    // mUiBackground.setOutlineColor(sf::Color::White);
    // mUiBackground.setOutlineThickness(1.0f);

    // 3. Health Bar (CoC Style: Green for Player)
    mPlayerHealthBarBack.setSize(sf::Vector2f(204.0f, 24.0f)); // Slightly larger for border
    mPlayerHealthBarBack.setFillColor(sf::Color(30, 30, 30)); // Dark Grey Background
    mPlayerHealthBarBack.setOutlineColor(sf::Color::Black);
    mPlayerHealthBarBack.setOutlineThickness(2.0f);
    mPlayerHealthBarBack.setPosition(28, 155);

    mPlayerHealthBarFront.setSize(sf::Vector2f(200.0f, 20.0f));
    mPlayerHealthBarFront.setFillColor(sf::Color(0, 200, 0)); // Bright Green
    mPlayerHealthBarFront.setPosition(30, 157); // Centered inside Back

    // 4. Text Elements (With Outlines for "Clash" look)
    auto styleText = [&](sf::Text& text, int size, sf::Color color, float x, float y) {
        text.setFont(mFont);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(2.5f); // Thick outline
        text.setPosition(x, y);
    };

    styleText(mLevelText, 20, sf::Color(255, 200, 0), 30, 20); // Gold Color
    mLevelText.setString("Level 1");

    styleText(mTimerText, 20, sf::Color::White, 30, 55);
    mTimerText.setString("Time: 60");

    styleText(mScoreText, 20, sf::Color::White, 30, 90);
    mScoreText.setString("Score: 0");

    // HP Label inside the bar or above? Let's put it inside or just remove it if bar is obvious.
    // Let's keep it small above.
    styleText(mHealthText, 14, sf::Color::White, 30, 130);
    mHealthText.setOutlineThickness(1.5f);
    mHealthText.setString("HP");
    
    // NEW: Win Text for Boss Level
    styleText(mWinText, 50, sf::Color::Green, 400, 300);
    mWinText.setOutlineThickness(4.0f);
    mWinText.setString("BOSS DEFEATED!\nGO TO RIGHT EDGE!");

    // --- COLLECTIBLES SETUP ---
    // Try the path relative to workspace root first
    if (!mCoinTexture.loadFromFile("assets/icons/coin.png") && 
        !mCoinTexture.loadFromFile("FinalProject/assets/icons/coin.png")) {
        std::cerr << "ERROR: Failed to load coin.png" << std::endl;
    }
    if (!mKeyTexture.loadFromFile("assets/icons/key.png") && 
        !mKeyTexture.loadFromFile("FinalProject/assets/icons/key.png")) {
        std::cerr << "ERROR: Failed to load key.png" << std::endl;
    }

    // UI for Collectibles
    mCoinIcon.setTexture(mCoinTexture);
    // Assuming icons are ~64x64, scale to ~32x32
    mCoinIcon.setScale(0.5f, 0.5f); 
    mCoinIcon.setPosition(30, 200); // Moved down slightly

    styleText(mCoinCountText, 18, sf::Color::Yellow, 75, 205); // Aligned with icon center
    mCoinCountText.setString("0/5");

    mKeyIcon.setTexture(mKeyTexture);
    mKeyIcon.setScale(0.5f, 0.5f); 
    mKeyIcon.setPosition(30, 245); // Spaced out

    styleText(mKeyCountText, 18, sf::Color::Cyan, 75, 250); // Aligned with icon center
    mKeyCountText.setString("0/3");

    // Initialize Pause Buttons
    float btnW = 300.0f;
    float btnH = 60.0f;
    float centerX = mWindow.getSize().x / 2.0f;
    float centerY = mWindow.getSize().y / 2.0f;
    
    initButton(mResumeButton, "RESUME", centerX, centerY - 50.0f, btnW, btnH);
    initButton(mMainMenuButton, "MAIN MENU", centerX, centerY + 50.0f, btnW, btnH);

    // Initialize Game Over Buttons
    float goBtnW = 250.0f;
    initButton(mRestartButton, "RESTART [R]", centerX - 140.0f, centerY + 50.0f, goBtnW, btnH);
    initButton(mGameOverMenuButton, "MENU [M]", centerX + 140.0f, centerY + 50.0f, goBtnW, btnH);
}


// ==================================================
// DESTRUCTOR (CLEANUP)
// ==================================================
Game::~Game() {
    delete mMenu;
    delete mPlayer;
    if (mBackground) delete mBackground;
    if (mParallaxBackground) delete mParallaxBackground;
    if (mGateway) delete mGateway;

    for (auto* enemy : mEnemies) {
        delete enemy;
    }
    mEnemies.clear();
    // mBoss is deleted via mEnemies vector, so we just null the pointer
    mBoss = nullptr; 
}


// ==================================================
// UPDATE LOOP
// ==================================================
void Game::update(sf::Time deltaTime) {
    if (deltaTime.asSeconds() > 0.1f) {
        deltaTime = sf::seconds(0.1f);
    }

    if (mState == STATE_PAUSED) {
        // Slow timer (3x slower)
        mLevelTimer -= deltaTime.asSeconds() / 3.0f;
        if (mLevelTimer < 0.0f) {
            mLevelTimer = 0.0f;
            mState = STATE_GAME_OVER;
        }
        mTimerText.setString("Time: " + to_string((int)mLevelTimer));
        return; // Stop other updates
    }

    if (mState == STATE_PLAYING) {

        // 1. PLAYER PHYSICS & COLLISION
        mPlayer->update(deltaTime);
        mPlayer->setIsOnGround(false);

        for (const auto& plat : mPlatforms) {
            mPlayer->handleCollision(plat.getGlobalBounds());
        }

        // 2. PLAYER BOUNDARY CHECK
        sf::Vector2f playerPos = mPlayer->getWorldPosition();
        float playerVisualWidth = mPlayer->getVisualWidth();

        if (playerPos.x < 0.0f) playerPos.x = 0.0f;
        if (playerPos.x > mLevelWidth - playerVisualWidth) {
            playerPos.x = mLevelWidth - playerVisualWidth;
        }
        mPlayer->setWorldPosition(playerPos);

        // 3. CAMERA SCROLLING
        float cameraTargetX = playerPos.x - (mWindow.getSize().x / 2.0f);
        if (cameraTargetX < 0.0f) cameraTargetX = 0.0f;
        float maxCameraX = mLevelWidth - mWindow.getSize().x;
        if (maxCameraX < 0.0f) maxCameraX = 0.0f;
        if (cameraTargetX > maxCameraX) cameraTargetX = maxCameraX;
        mCameraPosition.x = cameraTargetX;

        // 4. UI TIMER UPDATE
        mLevelTimer -= deltaTime.asSeconds();
        if (mLevelTimer < 0.0f) {
            mLevelTimer = 0.0f;
            // Time Up -> Game Over
            mState = STATE_GAME_OVER; 
            mGameOverReason = "Time Finished";
        }
        mTimerText.setString("Time: " + to_string((int)mLevelTimer));
        mScoreText.setString("Score: " + to_string(mScore));
        
        // Update Health Bar
        float hpPct = static_cast<float>(mPlayer->getHealth()) / 100.0f; // Assuming 100 max health
        if (hpPct < 0) hpPct = 0;
        mPlayerHealthBarFront.setSize(sf::Vector2f(200.0f * hpPct, 20.0f));

        // --- COLLECTIBLE COLLISION ---
        for (auto& item : mCollectibles) {
            if (!item.collected && item.getHitbox().intersects(mPlayer->getBodyHitbox())) {
                item.collected = true;
                if (item.type == COLLECTIBLE_COIN) {
                    mCoinsCollected++;
                    mScore += 50; // Bonus score
                    mCoinCountText.setString(to_string(mCoinsCollected) + "/5");
                    if (mCoinsCollected >= 5) mCoinCountText.setFillColor(sf::Color::Green);
                } else {
                    mKeysCollected++;
                    mScore += 100; // Bonus score
                    mKeyCountText.setString(to_string(mKeysCollected) + "/3");
                    if (mKeysCollected >= 3) mKeyCountText.setFillColor(sf::Color::Green);
                }
            }
        }

        // 5. LEVEL COMPLETION CHECK
        // Update Gateway
        bool enemiesCleared = mEnemies.empty();
        // If Boss level, ensure boss is dead (which is handled by mEnemies.empty() if boss is in list, 
        // but let's be safe since mBoss is a separate pointer too)
        if ((mCurrentLevel == 1 || mCurrentLevel == 2 || mCurrentLevel == 3) && mBoss != nullptr && !mBoss->isDead()) {
            enemiesCleared = false;
        }

        // NEW: Check Collectibles
        bool collectiblesCollected = (mCoinsCollected >= 5 && mKeysCollected >= 3);
        
        if (mGateway) {
            mGateway->update(deltaTime, mScore, enemiesCleared && collectiblesCollected);
            
            // Check Collision with Active Gateway
            if (mGateway->isActive() && mGateway->getHitbox().intersects(mPlayer->getBodyHitbox())) {
                 if (mCurrentLevel < 5) {
                    // Unlock next character logic
                    if (mUnlockedCharacters < mCurrentLevel + 1) {
                        mUnlockedCharacters = mCurrentLevel + 1;
                    }
                    // Load Next Level
                    loadLevel(mCurrentLevel + 1);
                } else {
                    // Game Won
                    mState = STATE_MENU; 
                }
            }
        }

        // 6. ENEMY UPDATES & COLLISION
        for (auto it = mEnemies.begin(); it != mEnemies.end(); ) {
            Enemy* enemy = *it;

            // Update Enemy
            enemy->update(deltaTime, playerPos, mPlatforms);

            // Bosses usually ignore platform collision (flying), 
            // but standard enemies need it.
            // We can use dynamic_cast to check if it's NOT a boss if we want logic separation
            if (!dynamic_cast<Level1Boss*>(enemy)) {
                 for (const auto& plat : mPlatforms) {
                    enemy->handleCollision(plat.getGlobalBounds());
                }
            }

            // --- COMBAT LOGIC ---
            
            // A. Player Attacks Enemy
            if (mPlayer->isAttacking()) {
                if (mPlayer->getAttackHitbox().intersects(enemy->getBodyHitbox())) {
                    // Deal damage only if enemy isn't already dead/invincible
                    
                    // Calculate Distance-Based Damage
                    float distance = std::abs(mPlayer->getWorldPosition().x - enemy->getPosition().x);
                    int damage = 25; // Base damage

                    // Closer = More Damage
                    // Hitbox width is roughly 100-200. 
                    if (distance < 100.0f) damage = 40; // Critical Hit
                    else if (distance < 200.0f) damage = 30; // Strong Hit
                    else damage = 20; // Weak Hit (Tip)

                    // BOSS LOGIC: Only X (Type 2) and C (Type 3) damage the boss
                    if (dynamic_cast<Level1Boss*>(enemy)) {
                        int attackType = mPlayer->getAttackType();
                        if (attackType == 2 || attackType == 3) {
                            enemy->takeDamage(damage);
                        }
                    } 
                    else {
                        // Normal enemies take damage from everything
                        enemy->takeDamage(damage); 
                    }
                }
            }

            // B. Enemy Attacks Player
            if (!enemy->isDead() && enemy->isAttacking()) {
                if (enemy->getBodyHitbox().intersects(mPlayer->getBodyHitbox())) {
                    mPlayer->takeDamage(10); 
                }
            }

            // C. Remove Dead Enemies
            if (enemy->isDead()) {
                // Check if this was the boss
                if (enemy == mBoss) {
                    mBoss = nullptr; // Boss is dead
                    mScore += 1000;  // Big points for boss
                } else {
                    mScore += 100;
                }

                delete enemy;
                it = mEnemies.erase(it);
                continue;
            }

            ++it;
        }

        // Check Player Death
        if (mPlayer->isDead()) {
            cout << "Player Died!" << endl;
            mState = STATE_GAME_OVER; 
            mGameOverReason = "Player Died";
        }
    }
}

// ==================================================
// RENDER LOOP
// ==================================================
void Game::render() {
    mWindow.clear();

    if (mState == STATE_MENU) {
        mMenu->render(mWindow);
        
        // Draw Selected Character Info
        sf::Text charText;
        charText.setFont(mFont);
        charText.setCharacterSize(20);
        charText.setFillColor(sf::Color::Yellow);
        charText.setOutlineColor(sf::Color::Black);
        charText.setOutlineThickness(2.0f);
        
        string txt = "Selected Character: Player " + to_string(mSelectedCharacter);
        if (mUnlockedCharacters < 2) txt += " (Unlock Level 2 for Player 2)";
        else txt += " (Press C to Switch)";
        
        charText.setString(txt);
        sf::FloatRect b = charText.getLocalBounds();
        charText.setOrigin(b.left + b.width/2.0f, b.top + b.height/2.0f);
        charText.setPosition(mWindow.getSize().x / 2.0f, mWindow.getSize().y - 50.0f);
        
        mWindow.draw(charText);
    }
    else if (mState == STATE_GAME_OVER) {
        // Draw Game Over Screen
        sf::Text title("GAME OVER", mFont, 80);
        title.setFillColor(sf::Color::Red);
        sf::FloatRect tr = title.getLocalBounds();
        title.setOrigin(tr.left + tr.width/2.0f, tr.top + tr.height/2.0f);
        title.setPosition(mWindow.getSize().x/2.0f, mWindow.getSize().y/3.0f);
        
        // Reason Text
        sf::Text reason(mGameOverReason, mFont, 30);
        reason.setFillColor(sf::Color::White);
        sf::FloatRect rr = reason.getLocalBounds();
        reason.setOrigin(rr.left + rr.width/2.0f, rr.top + rr.height/2.0f);
        reason.setPosition(mWindow.getSize().x/2.0f, mWindow.getSize().y/3.0f + 60.0f);

        mWindow.draw(title);
        mWindow.draw(reason);

        // Draw Buttons
        drawButton(mRestartButton);
        drawButton(mGameOverMenuButton);
    }
    else if (mState == STATE_PAUSED) {
        // Draw Game (Frozen)
        if (mParallaxBackground) mParallaxBackground->render(mWindow, mCameraPosition);
        else if (mBackground) mBackground->render(mWindow, mCameraPosition);
        
        // Draw entities... (Optional, but nice to see where you paused)
        // For simplicity, let's just draw a dark overlay and text
        
        sf::RectangleShape overlay(sf::Vector2f(mWindow.getSize().x, mWindow.getSize().y));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        mWindow.draw(overlay);

        sf::Text pauseText("PAUSED", mFont, 80);
        sf::FloatRect pr = pauseText.getLocalBounds();
        pauseText.setOrigin(pr.left + pr.width/2.0f, pr.top + pr.height/2.0f);
        pauseText.setPosition(mWindow.getSize().x/2.0f, mWindow.getSize().y/4.0f);
        mWindow.draw(pauseText);

        // Draw Buttons
        drawButton(mResumeButton);
        drawButton(mMainMenuButton);

        // Draw Timer (to show it's still running)
        mWindow.draw(mTimerText);
    }
    else if (mState == STATE_PLAYING) {

        // 1. Background
        if (mParallaxBackground) {
            mParallaxBackground->render(mWindow, mCameraPosition);
        }
        else if (mBackground) {
            mBackground->render(mWindow, mCameraPosition);
        }

        // 2. Platforms
        sf::View gameView = mWindow.getDefaultView();
        gameView.setCenter(mCameraPosition.x + mWindow.getSize().x / 2.0f, mWindow.getSize().y / 2.0f);
        mWindow.setView(gameView);

        for (const auto& plat : mPlatforms) {
            mWindow.draw(plat);
        }

        // Draw Collectibles
        for (const auto& item : mCollectibles) {
            if (!item.collected) {
                mWindow.draw(item.sprite);
            }
        }

        // 3. Entities
        mWindow.setView(mWindow.getDefaultView()); // Reset view for Manual Offset Rendering

        if (mGateway) {
            mGateway->render(mWindow, mCameraPosition);
        }

        mPlayer->render(mWindow, mCameraPosition);

        for (auto* enemy : mEnemies) {
            enemy->render(mWindow, mCameraPosition);
        }

        // 4. UI
        mWindow.draw(mUiBackground);
        
        mWindow.draw(mLevelText);
        mWindow.draw(mTimerText);
        mWindow.draw(mScoreText);
        
        // Draw Collectible UI
        mWindow.draw(mCoinIcon);
        mWindow.draw(mCoinCountText);
        mWindow.draw(mKeyIcon);
        mWindow.draw(mKeyCountText);
        
        mWindow.draw(mHealthText); // "HP" label
        mWindow.draw(mPlayerHealthBarBack);
        mWindow.draw(mPlayerHealthBarFront);
        
        // Show "Boss Defeated" hint if Boss is dead in Level 1
        if ((mCurrentLevel == 1 || mCurrentLevel == 2 || mCurrentLevel == 3) && mBoss == nullptr) {
            mWindow.draw(mWinText);
        }
    }

    mWindow.display();
}

// ==================================================
// RUN & EVENTS
// ==================================================
void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        processEvents();
        update(clock.restart());
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }

        if (mState == STATE_MENU) {
            int action = mMenu->handleInput(mWindow);
            if (action == 1) { // Start / Level 1
                mState = STATE_PLAYING;
                AudioManager::getInstance().stopMusic();
                AudioManager::getInstance().playMusic("assets/audio/bgmusic.mp3");
                loadLevel(1);
            }
            else if (action == 2) { // Level 2
                mState = STATE_PLAYING;
                AudioManager::getInstance().stopMusic();
                AudioManager::getInstance().playMusic("assets/audio/bgmusic.mp3");
                loadLevel(2);
            }
            else if (action == 3) { // Level 3
                mState = STATE_PLAYING;
                AudioManager::getInstance().stopMusic();
                AudioManager::getInstance().playMusic("assets/audio/bgmusic.mp3");
                loadLevel(3);
            }
            else if (action == -1) { // Quit
                mWindow.close();
            }

            // Character Selection Input
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::C) {
                    if (mUnlockedCharacters >= 2) {
                        mSelectedCharacter = (mSelectedCharacter == 1) ? 2 : 1;
                    }
                }
            }
        }
        else if (mState == STATE_PLAYING) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    mState = STATE_PAUSED;
                }
            }
        }
        else if (mState == STATE_PAUSED) {
            // Mouse Interaction
            sf::Vector2i mousePos = sf::Mouse::getPosition(mWindow);
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);
            bool isClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            static bool wasPressed = false;
            bool clickedNow = isClick && !wasPressed;
            wasPressed = isClick;

            // Resume Button
            if (mResumeButton.shape.getGlobalBounds().contains(worldPos)) {
                mResumeButton.isHovered = true;
                if (clickedNow) mState = STATE_PLAYING;
            } else {
                mResumeButton.isHovered = false;
            }

            // Main Menu Button
            if (mMainMenuButton.shape.getGlobalBounds().contains(worldPos)) {
                mMainMenuButton.isHovered = true;
                if (clickedNow) mState = STATE_MENU;
            } else {
                mMainMenuButton.isHovered = false;
            }

            // Keyboard Interaction
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    mState = STATE_MENU;
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    mState = STATE_PLAYING;
                }
            }
        }
        else if (mState == STATE_GAME_OVER) {
            // Mouse Interaction
            sf::Vector2i mousePos = sf::Mouse::getPosition(mWindow);
            sf::Vector2f worldPos = mWindow.mapPixelToCoords(mousePos);
            bool isClick = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            static bool wasPressedGO = false;
            bool clickedNow = isClick && !wasPressedGO;
            wasPressedGO = isClick;

            // Restart Button
            if (mRestartButton.shape.getGlobalBounds().contains(worldPos)) {
                mRestartButton.isHovered = true;
                if (clickedNow) {
                    mState = STATE_PLAYING;
                    loadLevel(mCurrentLevel);
                }
            } else {
                mRestartButton.isHovered = false;
            }

            // Menu Button
            if (mGameOverMenuButton.shape.getGlobalBounds().contains(worldPos)) {
                mGameOverMenuButton.isHovered = true;
                if (clickedNow) mState = STATE_MENU;
            } else {
                mGameOverMenuButton.isHovered = false;
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    // Restart current level
                    mState = STATE_PLAYING;
                    loadLevel(mCurrentLevel);
                }
                else if (event.key.code == sf::Keyboard::M) {
                    mState = STATE_MENU;
                }
            }
        }
    }
}

// ==================================================
// LOAD LEVEL LOGIC
// ==================================================
void Game::loadLevel(int levelIndex) {
    mCurrentLevel = levelIndex;
    
    // Difficulty: Time Limit
    // Level 1: 40s. Increase by 10s per level.
    mLevelTimer = 40.0f + (mCurrentLevel - 1) * 10.0f;

    // Clear Enemies & Platforms
    for (auto* enemy : mEnemies) delete enemy;
    mEnemies.clear();
    mPlatforms.clear();
    mBoss = nullptr; // Reset boss pointer
    
    // Clear Collectibles
    mCollectibles.clear();
    mCoinsCollected = 0;
    mKeysCollected = 0;
    mCoinCountText.setString("0/5");
    mCoinCountText.setFillColor(sf::Color::Yellow);
    mKeyCountText.setString("0/3");
    mKeyCountText.setFillColor(sf::Color::Cyan);

    if (mGateway) { delete mGateway; mGateway = nullptr; } // Reset Gateway

    // Reset Player
    if (mPlayer) delete mPlayer;
    if (mCurrentLevel == 1) {
        mPlayer = new Player(1);
    } else {
        mPlayer = new Player(mSelectedCharacter);
    }
    // Fix: Spawn higher to prevent Player 2 (taller) from spawning with feet below the ground
    mPlayer->setWorldPosition(sf::Vector2f(100.0f, 300.0f)); 
    mCameraPosition = sf::Vector2f(0, 0);
    
    // Update Text
    mLevelText.setString("Level: " + to_string(mCurrentLevel));

    // --- BACKGROUND ---
    if (mBackground) { delete mBackground; mBackground = nullptr; }
    if (mParallaxBackground) { delete mParallaxBackground; mParallaxBackground = nullptr; }

    if (mCurrentLevel == 2) {
        mParallaxBackground = new ParallaxBackground(
            static_cast<float>(mWindow.getSize().x),
            static_cast<float>(mWindow.getSize().y),
            "assets/map/Level2",
            8
        );
    }
    else if (mCurrentLevel == 3) {
        mParallaxBackground = new ParallaxBackground(
            static_cast<float>(mWindow.getSize().x),
            static_cast<float>(mWindow.getSize().y),
            "assets/map/Level3",
            5
        );
    }
    else if (mCurrentLevel == 4) {
        mParallaxBackground = new ParallaxBackground(
            static_cast<float>(mWindow.getSize().x),
            static_cast<float>(mWindow.getSize().y),
            "assets/map/Level4",
            10
        );
    }
    else {
        string bgPath = "assets/map/Level" + to_string(mCurrentLevel) + ".png";
        sf::Texture temp;
        if (!temp.loadFromFile(bgPath) && !temp.loadFromFile("../" + bgPath)) {
            bgPath = "assets/map/Level1.png"; // Fallback
        }
        mBackground = new StaticBackground(
            static_cast<float>(mWindow.getSize().x),
            static_cast<float>(mWindow.getSize().y),
            bgPath
        );
    }

    // --- PLATFORMS ---
    // Ground
    sf::Sprite ground;
    ground.setTexture(mPlatformTexture);
    ground.setTextureRect(sf::IntRect(0, 0, static_cast<int>(mLevelWidth), 100));
    ground.setScale(1.0f, 1.85f);
    ground.setPosition(0, 900 - 185);
    mPlatforms.push_back(ground);

    auto addPlatform = [&](float x, float y) {
        sf::Sprite p;
        p.setTexture(mPlatformTexture);
        p.setTextureRect(sf::IntRect(0, 0, 356, 100));
        p.setPosition(x, y);
        mPlatforms.push_back(p);
        return x;
    };

    // Platform Iterations (Standard Layout)
    float p2 = addPlatform(800.0f, 500.0f);
    float p3 = addPlatform(1300.0f, 270.0f);
    float p4 = addPlatform(1950.0f, 500.0f);
    float p5 = addPlatform(2500.0f, 270.0f);
    float p6 = addPlatform(3150.0f, 500.0f);
    
    // Combined Platform 7+8 (For Boss Arena)
    // 3800 starts P7. P8 starts immediately after.
    float p7 = addPlatform(3800.0f, 270.0f); 
    float p8 = addPlatform(3800.0f + 356.0f, 270.0f); 

    // --- GATEWAY ---
    // Rightmost edge on ground.
    // Level Width is 5000. Ground Y is 900 - 185 = 715.
    // Gateway Height is 104. If scaled 2x, it's 208.
    // Y = 715 - 208 = 507.
    // User requested: 50px below (507 + 50 = 557) and 20% larger.
    // X = 5000 - 200 (buffer) = 4800.
    mGateway = new Gateway(mLevelWidth - 300.0f, 557.0f);

    // --- COLLECTIBLES SPAWNING ---
    auto addCoin = [&](float x, float y) {
        mCollectibles.emplace_back(mCoinTexture, COLLECTIBLE_COIN, x, y);
    };
    auto addKey = [&](float x, float y) {
        mCollectibles.emplace_back(mKeyTexture, COLLECTIBLE_KEY, x, y);
    };

    // Spawn Collectibles (Difficult but possible locations)
    // Coins (5)
    addCoin(800.0f, 450.0f);   // Above P2
    addCoin(1950.0f, 450.0f);  // Above P4
    addCoin(3150.0f, 450.0f);  // Above P6
    addCoin(1600.0f, 650.0f);  // Ground, under gap
    addCoin(4500.0f, 650.0f);  // Ground, near end

    // Keys (3)
    addKey(1300.0f, 220.0f);   // High P3
    addKey(2500.0f, 220.0f);   // High P5
    addKey(3800.0f, 220.0f);   // Boss area P7

    // --- ENEMY SPAWNING ---
    
    // BOSS (Level 1 Only)
    if (mCurrentLevel == 1) {
        // Spawn centered on P7+P8
        // P7 start: 3800. P8 end: 3800 + 356 + 356 = 4512. Center ~ 4156.
        // Y = 200 (Flying high)
        mBoss = new Level1Boss(4156.0f, 200.0f);
        mEnemies.push_back(mBoss);
    }
    else if (mCurrentLevel == 2) {
        // Spawn on P7+P8 (3800 to 4512)
        // Y = 270 (Platform Y) - 64 (Half Height) = 206.
        // Center X = 4156.
        mBoss = new Level2Boss(4156.0f, 206.0f, 3800.0f, 4512.0f);
        mEnemies.push_back(mBoss);
    }
    else if (mCurrentLevel == 3) {
        // Spawn on P7+P8 (3800 to 4512)
        // Y = 270 (Platform Y) - 64 (Half Height) = 206.
        // Center X = 4156.
        std::cout << "--- SPAWNING LEVEL 3 BOSS AT (4156, 206) ---" << std::endl;
        mBoss = new Level3Boss(4156.0f, 206.0f, 3800.0f, 4512.0f);
        mEnemies.push_back(mBoss);
    }

    // Standard Enemies
    mEnemies.push_back(new Level1Enemy1(1000.0f, 630.0f));
    mEnemies.push_back(new Level1Enemy1(p2 + 50, 300.0f));
    mEnemies.push_back(new Level1Enemy1(p3 + 50, 100.0f));
    mEnemies.push_back(new Level1Enemy1(1600.0f, 630.0f));
    mEnemies.push_back(new Level1Enemy1(p4 + 50, 300.0f));
    mEnemies.push_back(new Level1Enemy1(p5 + 50, 100.0f));

    // Higher Level Difficulty scaling
    if (mCurrentLevel >= 2) {
        mEnemies.push_back(new Level2Enemy1(p4 + 50, 300.0f));
        mEnemies.push_back(new Level2Enemy1(p5 + 50, 100.0f));
        
        // Spawn "Mini Bosses" (Level 1 Boss as normal enemy)
        // Flying enemies, so spawn them in the air
        mEnemies.push_back(new Level1Boss(1500.0f, 200.0f, true)); 
        mEnemies.push_back(new Level1Boss(2800.0f, 150.0f, true));
    }
    if (mCurrentLevel >= 3) {
        mEnemies.push_back(new Level2Enemy1(p6 + 50, 300.0f));
        mEnemies.push_back(new Level2Enemy1(p7 + 50, 100.0f));
        mEnemies.push_back(new Level1Enemy1(2200.0f, 630.0f));
        
        // Spawn Level 2 Boss as Mini Boss
        // Ground enemy, so spawn on platform or ground
        // P6 is at 3150. Y=500.
        // Y = 500 - 64 = 436.
        mEnemies.push_back(new Level2Boss(3200.0f, 436.0f, 3150.0f, 3150.0f + 356.0f, true));
    }
}

void Game::initButton(Button& btn, const std::string& label, float x, float y, float width, float height) {
    btn.shape.setSize(sf::Vector2f(width, height));
    btn.shape.setOrigin(width / 2.0f, height / 2.0f);
    btn.shape.setPosition(x, y);
    btn.shape.setFillColor(sf::Color(50, 50, 50));
    btn.shape.setOutlineThickness(2.0f);
    btn.shape.setOutlineColor(sf::Color::White);

    btn.text.setFont(mFont);
    btn.text.setString(label);
    btn.text.setCharacterSize(24);
    btn.text.setFillColor(sf::Color::White);
    sf::FloatRect textRect = btn.text.getLocalBounds();
    btn.text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    btn.text.setPosition(x, y);
}

void Game::drawButton(Button& btn) {
    float radius = 15.0f;
    sf::Vector2f pos = btn.shape.getPosition();
    sf::Vector2f size = btn.shape.getSize();
    sf::Color color = btn.isHovered ? sf::Color(100, 100, 100) : sf::Color(50, 50, 50);

    sf::RectangleShape rect1(sf::Vector2f(size.x - 2 * radius, size.y));
    rect1.setOrigin(rect1.getSize().x / 2.0f, rect1.getSize().y / 2.0f);
    rect1.setPosition(pos);
    rect1.setFillColor(color);

    sf::RectangleShape rect2(sf::Vector2f(size.x, size.y - 2 * radius));
    rect2.setOrigin(rect2.getSize().x / 2.0f, rect2.getSize().y / 2.0f);
    rect2.setPosition(pos);
    rect2.setFillColor(color);

    sf::CircleShape corner(radius);
    corner.setFillColor(color);
    
    corner.setPosition(pos.x - size.x / 2.0f, pos.y - size.y / 2.0f); mWindow.draw(corner);
    corner.setPosition(pos.x + size.x / 2.0f - 2 * radius, pos.y - size.y / 2.0f); mWindow.draw(corner);
    corner.setPosition(pos.x - size.x / 2.0f, pos.y + size.y / 2.0f - 2 * radius); mWindow.draw(corner);
    corner.setPosition(pos.x + size.x / 2.0f - 2 * radius, pos.y + size.y / 2.0f - 2 * radius); mWindow.draw(corner);

    mWindow.draw(rect1);
    mWindow.draw(rect2);
    mWindow.draw(btn.text);
}