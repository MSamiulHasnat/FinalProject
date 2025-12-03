#include "Game.h"
#include "StaticBackground.h" 
#include <iostream>

using namespace std;

Game::Game()
    : mWindow(sf::VideoMode(1600, 900), "Final Project: Time Survivor")
    , mIsRunning(true)
    , mState(STATE_MENU)
    , mCameraPosition(0, 0)
{
    mWindow.setFramerateLimit(60);

    mMenu = new Menu(static_cast<float>(mWindow.getSize().x), static_cast<float>(mWindow.getSize().y));
    mPlayer = new Player(); // Player is created at (0,0)

    string level1_background_path = "assets/map/Level1.png";
    mBackground = new StaticBackground(
        static_cast<float>(mWindow.getSize().x),
        static_cast<float>(mWindow.getSize().y),
        level1_background_path
    );

    mLevelWidth = mBackground->getTextureWidth() * 3.0f;

    // --- THIS IS THE FIX ---
    // Use the robust 3-path loader for the platform texture
    string platformPath = "assets/map/Level1Land.png";
    string platformPath_p = "../" + platformPath;
    string platformPath_abs = "C:/Users/Samiul/source/repos/FinalProject/FinalProject/" + platformPath;

    if (!mPlatformTexture.loadFromFile(platformPath)) {
        if (!mPlatformTexture.loadFromFile(platformPath_p)) {
            if (!mPlatformTexture.loadFromFile(platformPath_abs)) {
                cerr << "--- CRITICAL FAILURE --- Could not load: " << platformPath << endl;
            }
            else {
                cout << "Loaded platform (Absolute)" << endl;
            }
        }
        else {
            cout << "Loaded platform (Debug)" << endl;
        }
    }
    else {
        cout << "Loaded platform (Release)" << endl;
    }
    // --- END FIX ---

    mPlatformTexture.setRepeated(true);

    // Create Ground (Unchanged)
    sf::Sprite ground;
    ground.setTexture(mPlatformTexture);
    ground.setTextureRect(sf::IntRect(0, 0, static_cast<int>(mLevelWidth), 100));
    ground.setScale(1.0f, 1.85f);
    ground.setPosition(0, 900 - 185);
    mPlatforms.push_back(ground);

    // Create plat2 (Unchanged)
    sf::Sprite plat2;
    plat2.setTexture(mPlatformTexture);
    plat2.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat2.setScale(356.0f / 356.0f, 100.0f / 100.0f);
    plat2.setPosition(800, 500);
    mPlatforms.push_back(plat2);

    // --- ADD THIS NEW PLATFORM ---
    sf::Sprite plat3;
    plat3.setTexture(mPlatformTexture);

    // Use the same 356x100 size as plat2
    plat3.setTextureRect(sf::IntRect(0, 0, 356, 100));
    plat3.setScale(356.0f / 356.0f, 100.0f / 100.0f);

    // --- Calculation ---
    // plat2 top:     500
    // Gap:           115
    // plat3 height:  100
    // New Y Position = 500 - 115 - 100 = 285
    plat3.setPosition(1300, 270); // 1200px from left, 285px from top

    mPlatforms.push_back(plat3);
}

Game::~Game() {
    delete mMenu;
    delete mPlayer;
    delete mBackground;
}

void Game::run() {
    sf::Clock clock;
    while (mWindow.isOpen()) {
        sf::Time deltaTime = clock.restart();
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        if (event.type == sf::Event::KeyPressed) {

            if (mState == STATE_MENU) {
                int action = mMenu->handleInput(event.key.code);
                if (action == 1) mState = STATE_PLAYING;
                else if (action == -1) mWindow.close();
            }
            else if (mState == STATE_PLAYING) {
                if (event.key.code == sf::Keyboard::Escape) {
                    mState = STATE_MENU;
                }

                // --- UPDATE THIS SECTION ---
                if (event.key.code == sf::Keyboard::Z) {
                    mPlayer->attack1(); // Renamed
                }
                if (event.key.code == sf::Keyboard::X) {
                    mPlayer->attack2(); // <-- NEW
                }
                if (event.key.code == sf::Keyboard::C) {
                    mPlayer->attack3(); // <-- NEW
                }
            }
        }
    }
}

// --- THIS FUNCTION FIXES BUG #2 ---
void Game::update(sf::Time deltaTime) {
    if (mState == STATE_PLAYING) {

        mPlayer->update(deltaTime);

        mPlayer->setIsOnGround(false);

        for (const auto& plat : mPlatforms) {
            mPlayer->handleCollision(plat.getGlobalBounds());
        }

        // --- UNIVERSAL BOUNDARY LOGIC ---
        sf::Vector2f playerPos = mPlayer->getWorldPosition();

        // --- THIS IS THE FIX ---
        // We get the VISUAL width, not the collision width
        float playerVisualWidth = mPlayer->getVisualWidth();

        // Check left boundary (Window edge)
        if (playerPos.x < 0.0f) {
            playerPos.x = 0.0f;
        }
        // Check right boundary (End of level)
        if (playerPos.x > mLevelWidth - playerVisualWidth) {
            playerPos.x = mLevelWidth - playerVisualWidth;
        }
        mPlayer->setWorldPosition(playerPos);
        // --- END FIX ---

        // 5. Update Camera
        float cameraTargetX = playerPos.x - (mWindow.getSize().x / 2.0f);

        // Camera Anchor (Left)
        if (cameraTargetX < 0.0f) {
            cameraTargetX = 0.0f;
        }

        // Camera Anchor (Right)
        float maxCameraX = mLevelWidth - mWindow.getSize().x;
        if (maxCameraX < 0.0f) {
            maxCameraX = 0.0f;
        }
        if (cameraTargetX > maxCameraX) {
            cameraTargetX = maxCameraX;
        }

        mCameraPosition.x = cameraTargetX;
    }
}

void Game::render() {
    mWindow.clear(sf::Color(20, 20, 30));

    if (mState == STATE_MENU) {
        mMenu->render(mWindow);
    }
    else if (mState == STATE_PLAYING) {
        mBackground->render(mWindow, mCameraPosition);

        for (const auto& plat : mPlatforms) {
            sf::Sprite drawSprite = plat;
            drawSprite.setPosition(
                plat.getPosition().x - mCameraPosition.x,
                plat.getPosition().y - mCameraPosition.y
            );
            mWindow.draw(drawSprite);
        }

        mPlayer->render(mWindow, mCameraPosition);
    }

    mWindow.display();
}