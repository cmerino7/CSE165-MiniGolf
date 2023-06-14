#pragma once
#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

class GameLevel
{
public:
    // level state
    std::vector<GameObject> *Obstacles;
    GameObject* L1Ob1, * L1Ob2;
    // constructor
    GameLevel() { }
    // loads level from file
    void Load(int level, unsigned int levelWidth, unsigned int levelHeight);
    // render level
    void Draw(SpriteRenderer& renderer, int level);
    // check if the level is completed (all non-solid tiles are destroyed)
    bool IsCompleted(bool done);
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif

