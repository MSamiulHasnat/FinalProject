#pragma once
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <string>
#include <map>
#include <iostream>
#include <vector>

class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    void playMusic(const std::string& filename, bool loop = true) {
        // List of paths to try
        std::vector<std::string> paths = {
            filename,
            "FinalProject/" + filename,
            "../" + filename,
            "../../" + filename,
            "assets/audio/bgmusic.ogg", // Fallback to OGG
            "assets/audio/bgmusic.wav"  // Fallback to WAV
        };

        // Suppress SFML error output temporarily to avoid spamming console
        std::streambuf* originalErr = sf::err().rdbuf();
        sf::err().rdbuf(NULL);

        bool loaded = false;
        for (const auto& path : paths) {
            if (mMusic.openFromFile(path)) {
                mMusic.setLoop(loop);
                mMusic.play();
                loaded = true;
                break;
            }
        }

        // Restore error output
        sf::err().rdbuf(originalErr);

        if (!loaded) {
            // If all failed, try one last time with error output enabled to show the reason
            std::cerr << "AudioManager: Failed to load music. Attempting one last time to show error:" << std::endl;
            if (!mMusic.openFromFile(filename)) {
                std::cerr << "Error: Could not load music file: " << filename << " (and alternatives)" << std::endl;
            }
        }
    }

    void stopMusic() {
        mMusic.stop();
    }

    void pauseMusic() {
        mMusic.pause();
    }

    void resumeMusic() {
        if (mMusic.getStatus() == sf::Music::Paused) {
            mMusic.play();
        }
    }

private:
    AudioManager() {} // Private constructor for Singleton
    sf::Music mMusic;
    
    // Prevent copying
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
};
