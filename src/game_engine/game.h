#ifndef __GAME_H__
#define __GAME_H__

// #include <src/commons/common_entities.h>
#include <iostream>

#include <src/io/game_io.h>
#include <src/commons/common_rendering.h>
#include <src/render_engine/renderer.h>
#include <src/game_engine/game.h>
#include <src/entities/entity.h>
// #include <src/entities/entity_light.h>
// #include <src/entities/ground.h>
#include <src/lighting/lighting.h>
#include <src/lighting/lightContainer.h>
#include <src/render_engine/display_manager.h>
#include <src/terrain/terrain.hpp>
#include <src/terrain/terrain_diamond_square.hpp>

#include <utils/include/iniparser.hpp>

class Game{
public:
    Game();
    ~Game();
    void init(string settingPath);
    void start();
protected:
    void terminate();
    int m_score;

    DisplayManager displayManager;
    Renderer renderer;
    GameIO gameIO;
    
    EntityDynamic* bird;
    std::vector<EntityStatic*> lightSources;
    std::vector<EntityStaticInstanced*> coins;
    std::vector<EntityStaticInstanced*> trees;
    TerrainDiaSq *terrain;

    // lights wil be pointed by EntityLight
    LightContainer lightContainer;
};

#endif