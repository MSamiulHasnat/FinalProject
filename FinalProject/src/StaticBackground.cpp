#include "StaticBackground.h"
#include <iostream>

using namespace std;

StaticBackground::StaticBackground(float windowWidth, float windowHeight, string texturePath)
    : mWindowWidth(windowWidth)      // <-- Initialize new variable
    , mWindowHeight(windowHeight)  // <-- Initialize new variable
{

    // --- Load the Texture ---
    string path1 = texturePath;
    string path2 = "../" + texturePath;
    string path3 = "C:/Users/Samiul/source/repos/FinalProject/FinalProject/" + texturePath;

    if (!mTexture.loadFromFile(path1)) {
        if (!mTexture.loadFromFile(path2)) {
            if (!mTexture.loadFromFile(path3)) {
                cerr << "--- CRITICAL FAILURE --- Could not load static background: " << texturePath << endl;
                return;
            }
        }
    }

    // --- THIS IS THE NEW LOGIC ---

    // 1. Tell the texture to repeat (wrap) instead of stop
    mTexture.setRepeated(true);

    // 2. Set the sprite to use this texture
    mSprite.setTexture(mTexture);

    // 3. Set the *area* of the texture to display.
    // We want to display a chunk exactly the size of the window (e.g., 1600x900)
    mSprite.setTextureRect(sf::IntRect(0, 0,
        static_cast<int>(mWindowWidth),
        static_cast<int>(mWindowHeight)
    ));

    // The sprite itself always stays at (0,0)
    mSprite.setPosition(0, 0);
}

StaticBackground::~StaticBackground() {}

void StaticBackground::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {

    // --- NEW RENDER LOGIC ---
    // Instead of moving the sprite, we move the *texture window*

    // We get the camera's X position (e.g., 1800)
    int texLeft = static_cast<int>(cameraOffset.x);

    // We tell the sprite: "Start sampling the texture from 1800px in"
    // Because the texture is set to repeated, it will wrap around automatically.
    mSprite.setTextureRect(sf::IntRect(
        texLeft,
        0, // We aren't scrolling vertically yet
        static_cast<int>(mWindowWidth),
        static_cast<int>(mWindowHeight)
    ));

    // We draw the sprite at (0,0)
    window.draw(mSprite);
}