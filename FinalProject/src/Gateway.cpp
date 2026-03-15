#include "Gateway.h"

Gateway::Gateway(float x, float y) 
    : mPosition(x, y)
    , mIsActive(false)
    , mCurrentFrame(0)
    , mFrameTime(0.1f)
    , mElapsedTime(0.0f)
{
    // Load Textures
    // Try multiple paths to be safe
    bool loadedOff = false;
    if (mTextureOff.loadFromFile("assets/map/gatewayoff.png")) loadedOff = true;
    else if (mTextureOff.loadFromFile("FinalProject/assets/map/gatewayoff.png")) loadedOff = true;
    else if (mTextureOff.loadFromFile("../assets/map/gatewayoff.png")) loadedOff = true;
    else if (mTextureOff.loadFromFile("C:/Users/Samiul/Desktop/FinalProject/FinalProject/assets/map/gatewayoff.png")) loadedOff = true;
    
    if (!loadedOff) std::cerr << "ERROR: Failed to load gatewayoff.png from any known path!" << std::endl;

    bool loadedOn = false;
    if (mTextureOn.loadFromFile("assets/map/gatewayon.png")) loadedOn = true;
    else if (mTextureOn.loadFromFile("FinalProject/assets/map/gatewayon.png")) loadedOn = true;
    else if (mTextureOn.loadFromFile("../assets/map/gatewayon.png")) loadedOn = true;
    else if (mTextureOn.loadFromFile("C:/Users/Samiul/Desktop/FinalProject/FinalProject/assets/map/gatewayon.png")) loadedOn = true;

    if (!loadedOn) std::cerr << "ERROR: Failed to load gatewayon.png from any known path!" << std::endl;

    // Setup Frames for ON animation (9 frames)
    // User specified 103.5 width. We'll use 103 for IntRect, but we might need to adjust if it drifts.
    // Usually sprite sheets are integer aligned.
    int frameWidth = 103; 
    int frameHeight = 104;
    
    for(int i=0; i<9; i++) {
        mFramesOn.push_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
    }

    mSprite.setTexture(mTextureOff);
    mSprite.setPosition(mPosition);
    mSprite.setScale(2.4f, 2.4f); // 20% larger than 2.0f (2.0 * 1.2 = 2.4)
}

Gateway::~Gateway() {}

void Gateway::update(sf::Time deltaTime, int score, bool enemiesCleared) {
    // Activation Logic
    // Condition: All enemies killed (and collectibles collected, passed via boolean)
    // REMOVED Score Requirement (was causing issues in Level 2 where max score < 1500)
    if (enemiesCleared) {
        mIsActive = true;
    } else {
        mIsActive = false;
    }

    if (mIsActive) {
        mSprite.setTexture(mTextureOn);
        
        // Animation
        mElapsedTime += deltaTime.asSeconds();
        if (mElapsedTime >= mFrameTime) {
            mElapsedTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % mFramesOn.size();
        }
        
        if (!mFramesOn.empty()) {
            mSprite.setTextureRect(mFramesOn[mCurrentFrame]);
        }
    } else {
        mSprite.setTexture(mTextureOff);
        // Reset to full image or first frame if it's a sheet (it's 1 frame)
        sf::Vector2u size = mTextureOff.getSize();
        mSprite.setTextureRect(sf::IntRect(0, 0, size.x, size.y));
    }
}

void Gateway::render(sf::RenderWindow& window, sf::Vector2f cameraOffset) {
    mSprite.setPosition(mPosition.x - cameraOffset.x, mPosition.y - cameraOffset.y);
    window.draw(mSprite);
}

sf::FloatRect Gateway::getHitbox() const {
    // Adjust hitbox for scale
    return sf::FloatRect(
        mPosition.x, 
        mPosition.y, 
        mSprite.getGlobalBounds().width, 
        mSprite.getGlobalBounds().height
    );
}
