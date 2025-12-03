#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// A simple structure to hold the settings for a specific level
// This allows us to easily create Level 1, 2, 3, 4, 5 just by changing numbers
struct LevelConfig {
    int id;                     // 1, 2, 3...
    float timeLimit;            // Seconds (60, 120...)
    int minScoreToUnlock;       // 1000
    int instantWinScore;        // 3000
    std::string bgTexturePath;  // "assets/levels/level1/background.png"
};

class Level {
public:
    // Constructor: Requires the window size (for background scaling) and the config
    Level(sf::RenderWindow& window, LevelConfig config);
    ~Level();

    // Returns true if the level is still running
    // Returns false if the level is finished (Win or Loss)
    void update(sf::Time deltaTime);
    void render(sf::RenderWindow& window);

    // Getters to let the Game Engine know what happened
    bool isCompleted() const { return mIsCompleted; }
    bool isFailed() const { return mIsFailed; }
    int getScore() const { return mCurrentScore; }

private:
    // Level Settings
    LevelConfig mConfig;

    // State Variables
    float mCurrentTime;
    int mCurrentScore;
    bool mIsCompleted;
    bool mIsFailed;

    // Visual Assets
    sf::Texture mBgTexture;
    sf::Sprite mBgSprite;

    // Font for UI (Timer/Score)
    sf::Font mUIFont;
    sf::Text mTimerText;
    sf::Text mScoreText;

    // Helper to update the UI text strings
    void updateUI();
};