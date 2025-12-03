#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "Player.h"
#include "StaticBackground.h"

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
    sf::Vector2f mCameraPosition; // Our camera's (X,Y)
    sf::RectangleShape mGoalBox;  // The green box

    bool mIsRunning;
    GameState mState;
    Menu* mMenu;
    
    // CORRECT PLACEMENT: Inside the class, under private
    Player* mPlayer; 
    StaticBackground* mBackground;

    float mLevelWidth; // Total width of the level (3x background)
    sf::Texture mPlatformTexture;
    vector<sf::Sprite> mPlatforms;
};