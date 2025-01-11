#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <utils/learnopenglPrerequisites.h>
#include <utils/glPrerequisites.h>
#include <src/entities/entity.h>
#include <src/entities/entity_light.h>
#include <src/terrain/terrain.hpp>
#include <src/terrain/terrain_diamond_square.hpp>
#include <src/lighting/lightContainer.h>
#include <src/render_engine/shadow.h>
#include <vector>

class Renderer{
public:
    Renderer();
    ~Renderer();

    void init(string settingPath);
    void draw(LightContainer &lightContainer, EntityDynamic* bird, std::vector<EntityStaticInstanced*> items, std::vector<EntityStaticInstanced*> trees, std::vector<EntityStatic*> lightSources, TerrainDiaSq* terrain);

    Camera* getCamera();
    void setWindow(GLFWwindow* window);
private:
    GLFWwindow *mainWindow;
    Camera mainCamera;

    Shader *dynamicShader;
    Shader *staticShader;
    Shader *instancingShader;
    Shader *groundShader;
    // Axis axis;

    Shadow *shadow = nullptr;
};
#endif