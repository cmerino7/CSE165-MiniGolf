#pragma once
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Represents the four possible (collision) directions
enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};
// Defines a Collision typedef that represents collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision; // <collision?, what direction?, difference vector center - closest point>
// Radius of the ball & hole object & ball mass
const float BALL_RADIUS = 12.5f;
const float BALL_MASS = 45.0f;
const float HOLE_SIZE = 25.0f;


// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    bool                    rMouseClick;
    unsigned int            Width, Height;
    double                  x1,x2,y1,y2,x,y;
    bool                    shoot = false;
    std::vector<GameLevel>  Levels; //vector of levels (ie Levels[i])
    unsigned int            Level;  //level number
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions(float dt);
    // reset
    void ResetLevel();
    void ResetBall();
    //friendo
};

#endif

