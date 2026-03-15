#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "Player.h"
#include "StaticBackground.h"
#include "Level1Enemy1.h"
#include "Level1Boss.h" // <--- NEW: Include Boss Header
#include "Gateway.h"    // <--- NEW: Include Gateway Header
#include "ParallaxBackground.h" // <--- NEW: Parallax Background
#include "AudioManager.h" // <--- NEW: Audio Manager
#include <vector>

using namespace std;

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_VICTORY
};

enum CollectibleType {
    COLLECTIBLE_COIN,
    COLLECTIBLE_KEY
};

struct Collectible {
    sf::Sprite sprite;
    CollectibleType type;
    bool collected;

    Collectible(const sf::Texture& texture, CollectibleType t, float x, float y) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        type = t;
        collected = false;
        // Scale if needed. Assuming 32x32 or similar, maybe scale up a bit?
        // Let's assume they are reasonably sized or scale them to be ~40x40
        // If original is large, we might need to scale down.
        // For now, let's keep 1.0f unless they are huge.
        // Actually, let's set origin to center for easier placement
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    }

    sf::FloatRect getHitbox() const {
        return sprite.getGlobalBounds();
    }
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void processEvents();
    void update(sf::Time deltaTime);
    void render();
    void loadLevel(int levelIndex);

    sf::RenderWindow mWindow;
    sf::Vector2f mCameraPosition;

    bool mIsRunning;
    GameState mState;
    Menu* mMenu;

    Player* mPlayer;
    StaticBackground* mBackground;
    ParallaxBackground* mParallaxBackground; // <--- NEW: Parallax Background for Level 2

    float mLevelWidth;
    sf::Texture mPlatformTexture;
    vector<sf::Sprite> mPlatforms;

    vector<Enemy*> mEnemies;
    Enemy* mBoss; // <--- NEW: Pointer to track the boss specifically
    Gateway* mGateway; // <--- NEW: Level Exit Gateway

    sf::Font mFont;
    
    // UI Elements
    sf::RectangleShape mUiBackground;
    sf::RectangleShape mPlayerHealthBarBack;
    sf::RectangleShape mPlayerHealthBarFront;
    
    sf::Text mTimerText;
    sf::Text mScoreText;
    sf::Text mLevelText;  // Display current level
    sf::Text mHealthText; // Display player health
    sf::Text mWinText;    // <--- NEW: "Level Unlocked" text

    float mLevelTimer; 
    int mScore;        
    int mCurrentLevel; 
    int mUnlockedCharacters; 
    int mSelectedCharacter; 

    // Pause Menu Buttons
    Button mResumeButton;
    Button mMainMenuButton;

    // Game Over Elements
    std::string mGameOverReason;
    Button mRestartButton;
    Button mGameOverMenuButton;

    // Collectibles
    sf::Texture mCoinTexture;
    sf::Texture mKeyTexture;
    std::vector<Collectible> mCollectibles;
    int mCoinsCollected;
    int mKeysCollected;

    // UI for Collectibles
    sf::Sprite mCoinIcon;
    sf::Sprite mKeyIcon;
    sf::Text mCoinCountText;
    sf::Text mKeyCountText;

    void initButton(Button& btn, const std::string& label, float x, float y, float width, float height);
    void drawButton(Button& btn);
};