#include "Level.h"
#include <iostream>
#include <cmath> // For rounding the timer
using namespace std;

Level::Level(sf::RenderWindow& window, LevelConfig config)
    : mConfig(config)
    , mCurrentTime(0.0f)
    , mCurrentScore(0) // Start at 0 score
    , mIsCompleted(false)
    , mIsFailed(false)
{
    // 1. Load Background
    if (!mBgTexture.loadFromFile(mConfig.bgTexturePath)) {
        // Fallback if image is missing: Create a colored placeholder
        std::cout << "Warning: Could not load background: " << mConfig.bgTexturePath << std::endl;
        // We will just leave it empty (black) or you could load a default
    }
    else {
        mBgSprite.setTexture(mBgTexture);

        // Scale background to fit window
        float scaleX = window.getSize().x / (float)mBgTexture.getSize().x;
        float scaleY = window.getSize().y / (float)mBgTexture.getSize().y;
        mBgSprite.setScale(scaleX, scaleY);
    }

    // 2. Load UI Font (Reuse the one from Menu or load new)
    // Note: In a big engine, we would use a "ResourceManager" so we don't load arial.ttf twice.
    // For this project, loading it again is fine.
    if (!mUIFont.loadFromFile("../assets/fonts/arial.ttf")) {
        mUIFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }

    // 3. Setup UI Text
    mTimerText.setFont(mUIFont);
    mTimerText.setCharacterSize(24);
    mTimerText.setFillColor(sf::Color::White);
    mTimerText.setPosition(10, 10);

    mScoreText.setFont(mUIFont);
    mScoreText.setCharacterSize(24);
    mScoreText.setFillColor(sf::Color::Yellow);
    mScoreText.setPosition(10, 40);
}

Level::~Level() {
    // Cleanup
}

void Level::update(sf::Time deltaTime) {
    // Stop updating if the level is already over
    if (mIsCompleted || mIsFailed) return;

    // 1. Update Timer
    mCurrentTime += deltaTime.asSeconds();
    float timeRemaining = mConfig.timeLimit - mCurrentTime;

    // 2. Update UI Strings
    updateUI();

    // --- WIN/LOSS LOGIC ---

    // CHECK: Time ran out
    if (timeRemaining <= 0) {
        if (mCurrentScore >= mConfig.minScoreToUnlock) {
            // WIN CONDITION 1: Survival Path (Time done + Enough Score)
            std::cout << "Level Complete: Survival Success!" << std::endl;
            mIsCompleted = true;
        }
        else {
            // LOSS: Time done but score too low
            std::cout << "Game Over: Time Up & Low Score!" << std::endl;
            mIsFailed = true;
        }
    }

    // CHECK: Skill Path (High Score instant win)
    if (mCurrentScore >= mConfig.instantWinScore) {
        // WIN CONDITION 2: Skill Path
        std::cout << "Level Complete: Skill Victory!" << std::endl;
        mIsCompleted = true;
    }

    // TODO: Later we will add Player Physics and Enemy updates here
}

void Level::render(sf::RenderWindow& window) {
    window.draw(mBgSprite);
    window.draw(mTimerText);
    window.draw(mScoreText);

    // Later: Draw Player and Enemies
}

void Level::updateUI() {
    // Calculate time remaining
    int timeRemaining = static_cast<int>(std::ceil(mConfig.timeLimit - mCurrentTime));
    if (timeRemaining < 0) timeRemaining = 0;

    mTimerText.setString("Time: " + std::to_string(timeRemaining) + "s / " + std::to_string((int)mConfig.timeLimit) + "s");
    mScoreText.setString("Score: " + std::to_string(mCurrentScore) + " / Target: " + std::to_string(mConfig.minScoreToUnlock));
}