#include "ParallaxBackground.h"
#include <iostream>

using namespace std;

ParallaxBackground::ParallaxBackground(float windowWidth, float windowHeight, const std::string& levelPath, int layerCount)
    : mWindowWidth(windowWidth)
    , mWindowHeight(windowHeight)
{
    // Load layers
    for (int i = 1; i <= layerCount; ++i) {
        sf::Texture texture;
        string filename = to_string(i) + ".png";
        string path1 = levelPath + "/" + filename;
        string path2 = "FinalProject/" + levelPath + "/" + filename;
        string path3 = "../" + levelPath + "/" + filename;
        
        // Try loading
        if (!texture.loadFromFile(path1)) {
            if (!texture.loadFromFile(path2)) {
                if (!texture.loadFromFile(path3)) {
                     // Absolute path fallback
                     string path4 = "C:/Users/Samiul/Desktop/FinalProject/FinalProject/" + levelPath + "/" + filename;
                     if (!texture.loadFromFile(path4)) {
                        cerr << "ERROR: Failed to load parallax layer: " << filename << " from " << levelPath << endl;
                        continue; 
                     }
                }
            }
        }

        texture.setRepeated(true);
        mTextures.push_back(texture);
    }

    // Setup Sprites and Factors
    // Layer 1 (Farthest) -> Factor 0.1
    // Layer N (Closest)  -> Factor 0.8
    for (size_t i = 0; i < mTextures.size(); ++i) {
        sf::Sprite sprite;
        sprite.setTexture(mTextures[i]);
        
        // Scale to fit window height
        float scaleY = mWindowHeight / static_cast<float>(mTextures[i].getSize().y);
        sprite.setScale(scaleY, scaleY); 
        
        mSprites.push_back(sprite);
        
        // Factor: 0.1 * (i + 1)
        mParallaxFactors.push_back(0.1f * (i + 1));
    }
}

ParallaxBackground::~ParallaxBackground() {
    // Textures are in vector, automatically destroyed.
}

void ParallaxBackground::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    for (size_t i = 0; i < mSprites.size(); ++i) {
        sf::Sprite& sprite = mSprites[i];
        float factor = mParallaxFactors[i];
        
        // Calculate offset based on camera X
        float xOffset = cameraOffset.x * factor;
        
        // We need to set the texture rect to "scroll"
        // The texture rect starts at xOffset / scaleX
        float scale = sprite.getScale().x;
        
        sprite.setTextureRect(sf::IntRect(
            static_cast<int>(xOffset / scale), 
            0, 
            static_cast<int>(mWindowWidth / scale), 
            static_cast<int>(mTextures[i].getSize().y) // Use full texture height
        ));
        
        sprite.setPosition(0, 0); // Always draw at 0,0 of the window (since we move texture rect)
        window.draw(sprite);
    }
}
