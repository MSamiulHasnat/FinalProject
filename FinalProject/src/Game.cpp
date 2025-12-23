#include "Game.h"
#include "Level1Enemy1.h" // Required for spawning enemies
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
{
    mWindow.setFramerateLimit(60);

    // Seed Randomness
    srand(static_cast<unsigned int>(time(NULL)));

    mMenu = new Menu(static_cast<float>(mWindow.getSize().x), static_cast<float>(mWindow.getSize().y));
    mPlayer = new Player();

    // --- BACKGROUND ---
    string level1_background_path = "assets/map/Level1.png";
    mBackground = new StaticBackground(
        static_cast<float>(mWindow.getSize().x),
        static_cast<float>(mWindow.getSize().y),
        level1_background_path
    );
    mLevelWidth = mBackground->getTextureWidth() * 3.0f;

    // --- TEXTURES ---
    string platformPath = "assets/map/Level1Land.png";
    if (!mPlatformTexture.loadFromFile(platformPath)) {
        if (!mPlatformTexture.loadFromFile("../" + platformPath)) {
            // Fallback absolute path (keep your specific path here if needed)
            mPlatformTexture.loadFromFile("C:/Users/Samiul/source/repos/FinalProject/FinalProject/" + platformPath);
        }
    }
    mPlatformTexture.setRepeated(true);

    // ==========================================
    // --- MANUAL PLATFORM CONFIGURATION ---
    // ==========================================

    // 1. GROUND (Base)
    sf::Sprite ground;
    ground.setTexture(mPlatformTexture);
    ground.setTextureRect(sf::IntRect(0, 0, static_cast<int>(mLevelWidth), 100));
    ground.setScale(1.0f, 1.85f);
    ground.setPosition(0, 900 - 185);
    mPlatforms.push_back(ground);

    // --- ITERATION 1 (0px - 1600px) ---
    float plat2_x = 800.0f;   // Middle
    float plat3_x = 1300.0f;  // High

    sf::Sprite plat2;
    plat2.setTexture(mPlatformTexture);
    plat2.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat2.setPosition(plat2_x, 500);
    mPlatforms.push_back(plat2);

    sf::Sprite plat3;
    plat3.setTexture(mPlatformTexture);
    plat3.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat3.setPosition(plat3_x, 270);
    mPlatforms.push_back(plat3);


    // --- ITERATION 2 (1600px - 3200px) ---
    float plat4_x = 1950.0f;
    float plat5_x = 2500.0f;

    sf::Sprite plat4;
    plat4.setTexture(mPlatformTexture);
    plat4.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat4.setPosition(plat4_x, 500);
    mPlatforms.push_back(plat4);

    sf::Sprite plat5;
    plat5.setTexture(mPlatformTexture);
    plat5.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat5.setPosition(plat5_x, 270);
    mPlatforms.push_back(plat5);


    // --- ITERATION 3 (3200px - 4800px) ---
    float plat6_x = 3150.0f;
    float plat7_x = 3800.0f;

    sf::Sprite plat6;
    plat6.setTexture(mPlatformTexture);
    plat6.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat6.setPosition(plat6_x, 500);
    mPlatforms.push_back(plat6);

    sf::Sprite plat7;
    plat7.setTexture(mPlatformTexture);
    plat7.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat7.setPosition(plat7_x, 270);
    mPlatforms.push_back(plat7);


    // ==========================================
    // --- UI SETUP ---
    // ==========================================
    if (!mFont.loadFromFile("assets/fonts/arial.ttf")) {
        if (!mFont.loadFromFile("../assets/fonts/arial.ttf")) {
            mFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
        }
    }

    mTimerText.setFont(mFont);
    mTimerText.setCharacterSize(40);
    mTimerText.setFillColor(sf::Color::White);
    mTimerText.setPosition(20, 20);
    mTimerText.setString("Time: 60");

    mScoreText.setFont(mFont);
    mScoreText.setCharacterSize(40);
    mScoreText.setFillColor(sf::Color::Yellow);
    mScoreText.setPosition(20, 70);
    mScoreText.setString("Score: 0");

    // ==========================================
    // --- ENEMY SPAWNING ---
    // ==========================================

    // 1. Ground Cat
    mEnemies.push_back(new Level1Enemy1(1000.0f, 630.0f));

    // 2. Iteration 1 Cats
    mEnemies.push_back(new Level1Enemy1(plat2_x + 50, 300.0f));
    mEnemies.push_back(new Level1Enemy1(plat3_x + 50, 100.0f));

    // 3. Iteration 2 Cats
    mEnemies.push_back(new Level1Enemy1(plat4_x + 50, 300.0f));
    mEnemies.push_back(new Level1Enemy1(plat5_x + 50, 100.0f));

    // 4. Iteration 3 Cats
    mEnemies.push_back(new Level1Enemy1(plat6_x + 50, 300.0f));
    mEnemies.push_back(new Level1Enemy1(plat7_x + 50, 100.0f));
}


// ==================================================
// DESTRUCTOR (CLEANUP)
// ==================================================
Game::~Game() {
    // Delete raw pointers created in the constructor
    delete mMenu;
    delete mPlayer;
    delete mBackground;

    // Delete all enemies in the vector
    for (auto* enemy : mEnemies) {
        delete enemy;
    }
    mEnemies.clear();
}


// ==================================================
// UPDATE LOOP
// ==================================================
void Game::update(sf::Time deltaTime) {
    // --- CLAMP DELTA TIME (Prevent physics explosion on lag) ---
    if (deltaTime.asSeconds() > 0.1f) {
        deltaTime = sf::seconds(0.1f);
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
            // TODO: Game Over Logic
        }
        mTimerText.setString("Time: " + to_string((int)mLevelTimer));
        mScoreText.setString("Score: " + to_string(mScore));

        // 5. ENEMY UPDATES & COLLISION
        for (auto* enemy : mEnemies) {

            // --- CHANGE: Pass 'mPlatforms' as the 3rd argument ---
            enemy->update(deltaTime, playerPos, mPlatforms);

            // Check Collision
            for (const auto& plat : mPlatforms) {
                enemy->handleCollision(plat.getGlobalBounds());
            }
        }
    }
}

// ==================================================
// RENDER LOOP (Assuming you have this part)
// ==================================================
void Game::render() {
    mWindow.clear();

    if (mState == STATE_MENU) {
        // FIX 1: Use render() instead of draw()
        mMenu->render(mWindow);
    }
    else if (mState == STATE_PLAYING) {

        // --- 1. Draw Background ---
        // FIX 2: Pass 'mCameraPosition' (Vector2f), not 'mCameraPosition.x' (float)
        // Background usually handles its own scrolling logic with the default view
        mBackground->render(mWindow, mCameraPosition);


        // --- 2. Draw Platforms (Raw Sprites) ---
        // Since platforms are just sprites at world coordinates, we must move the CAMERA 
        // to see them correctly.
        sf::View gameView = mWindow.getDefaultView();
        // Set center of view = Camera X + Half Screen Width
        gameView.setCenter(mCameraPosition.x + mWindow.getSize().x / 2.0f, mWindow.getSize().y / 2.0f);
        mWindow.setView(gameView);

        for (const auto& plat : mPlatforms) {
            mWindow.draw(plat);
        }


        // --- 3. Draw Entities (Player & Enemies) ---
        // Your Player and Enemy classes handle the offset math MANUALLY (World - Camera).
        // So we must switch BACK to the Default View (0,0) to avoid double-scrolling.
        mWindow.setView(mWindow.getDefaultView());

        // FIX 3: Pass 'mCameraPosition' to Player
        mPlayer->render(mWindow, mCameraPosition);

        for (auto* enemy : mEnemies) {
            // Enemy also needs Camera Position to draw correctly
            enemy->render(mWindow, mCameraPosition);
        }


        // --- 4. Draw UI ---
        // UI is always drawn with Default View (already set above)
        mWindow.draw(mTimerText);
        mWindow.draw(mScoreText);
    }

    mWindow.display();
}

// ==================================================
// RUN & EVENTS (Standard Boilerplate)
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
            // Simple Menu Input
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    mState = STATE_PLAYING;
                }
            }
        }
    }
}