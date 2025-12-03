# Final Project - 2D Platformer Game

## Overview
This project is a 2D platformer game developed in C++ using the SFML (Simple and Fast Multimedia Library). It features a robust game engine with a state machine-based player controller, level management, and a camera system. The game is designed with Object-Oriented Programming (OOP) principles, ensuring modularity and maintainability.

### Gameplay Demo
[![Gameplay Demo](http://img.youtube.com/vi/GqKTSETgcbw/0.jpg)](https://youtu.be/GqKTSETgcbw)

## Features
- **Dynamic Player Controller**: A complex state machine handling various actions such as Idle, Walk, Run, Jump, Fall, Slide, Roll, and multiple Attack patterns.
- **Physics Engine**: Custom implementation of gravity, velocity, and collision detection (AABB) for precise platforming mechanics.
- **Level System**: Configurable levels with time limits, score goals, and win/loss conditions.
- **Camera System**: A 2D camera that smoothly tracks the player's movement across the level.
- **Menu System**: Interactive main menu for game navigation.
- **Animation System**: Frame-based sprite animation supporting multiple states and directions.

## Technical Architecture

### Core Components
The project is structured around several key classes:

1.  **`Game` Class (`Game.h`, `Game.cpp`)**
    -   **Role**: The central engine that manages the game loop (`run`, `update`, `render`).
    -   **Responsibilities**:
        -   Handles the main game loop and event processing.
        -   Manages game states (`STATE_MENU`, `STATE_PLAYING`, `STATE_GAMEOVER`).
        -   Controls the camera system (`mCameraPosition`) to follow the player.
        -   Orchestrates the interaction between the Player, Level, and Background.

2.  **`Player` Class (`Player.h`, `Player.cpp`)**
    -   **Role**: Encapsulates all player logic, physics, and rendering.
    -   **Key Features**:
        -   **State Machine**: Uses an `enum class PlayerState` to manage transitions between states like `IDLE`, `RUN`, `JUMP`, `ATTACK`, etc.
        -   **Physics**: Manages velocity, gravity application, and ground detection.
        -   **Animation**: Handles sprite sheet slicing and frame updates based on the current state and direction.
        -   **Collision**: Provides hitboxes (`getFeetHitbox`, `getBodyHitbox`) for interaction with the world.

3.  **`Level` Class (`Level.h`, `Level.cpp`)**
    -   **Role**: Manages specific level data and logic.
    -   **Key Features**:
        -   **Configuration**: Uses `LevelConfig` struct to define unique parameters for each level (time limit, unlock score, background).
        -   **Game Logic**: Tracks win/loss conditions based on time and score.
        -   **UI**: Renders level-specific UI elements like the timer and score counter.

4.  **`Menu` Class (`Menu.h`, `Menu.cpp`)**
    -   **Role**: Handles the initial game state.
    -   **Responsibilities**: Renders the title screen and processes input to start the game or exit.

5.  **`StaticBackground` Class (`StaticBackground.h`, `StaticBackground.cpp`)**
    -   **Role**: Manages the background rendering.
    -   **Responsibilities**: Draws the background texture, accounting for camera movement to create a seamless visual experience.

### Directory Structure
```
FinalProject/
├── src/                # Source code files (.cpp and .h)
│   ├── Game.cpp/h
│   ├── Level.cpp/h
│   ├── Main.cpp
│   ├── Menu.cpp/h
│   ├── Player.cpp/h
│   └── StaticBackground.cpp/h
├── assets/             # Game assets
│   ├── enemy/
│   ├── fonts/
│   ├── map/
│   └── player/
├── include/            # External library headers (SFML)
└── lib/                # External library binaries
```

## Controls
-   **Movement**: [A] / [D] or Left/Right Arrow Keys
-   **Jump**: [Space], [W], or Up Arrow Key
-   **Attack 1**: [Z]
-   **Attack 2**: [X]
-   **Attack 3**: [C]
-   **Slide**: [S] or Down Arrow Key (while running)
-   **Roll**: [R] (while moving)

## Requirements
-   **OS**: Windows (x64)
-   **IDE**: Visual Studio 2019/2022
-   **Dependencies**: SFML (included in `include/` and `lib/` directories)

## Build Instructions
1.  Open `FinalProject.sln` in Visual Studio.
2.  Ensure the build configuration is set to **x64** (Debug or Release).
3.  Build the solution (Ctrl+Shift+B).
4.  Run the application (F5).

## Credits
Developed by MSamiulHasnat.