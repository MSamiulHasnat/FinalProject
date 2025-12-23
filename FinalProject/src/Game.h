#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "Player.h"
#include "StaticBackground.h"
#include "Level1Enemy1.h"
#include <vector>

using namespace std;

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER
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

    sf::RenderWindow mWindow;
    sf::Vector2f mCameraPosition;

    bool mIsRunning;
    GameState mState;
    Menu* mMenu;

    Player* mPlayer;
    StaticBackground* mBackground;

    float mLevelWidth;
    sf::Texture mPlatformTexture;
    vector<sf::Sprite> mPlatforms;

    vector<Enemy*> mEnemies;

    sf::Font mFont;
    sf::Text mTimerText;
    sf::Text mScoreText;

    float mLevelTimer; // Counts down from 60
    int mScore;        // Starts at 0
};