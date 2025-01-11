#ifndef __SHADOW_H__
#define __SHADOW_H__

#include <vector>
#include <memory>
#include <utils/glPrerequisites.h>
#include <src/lighting/lightContainer.h>
#include <src/entities/entity.h>
#include <src/terrain/terrain_diamond_square.hpp>

class Shadow
{
private:
    LightContainer *m_lightContainer;
    unsigned int m_numDepthMaps;
    unsigned int *m_depthMapTextures;
    unsigned int *m_depthMapFrames;

    glm::ivec3 m_numLights;

    float m_near, m_far;

    void draw(glm::mat4 lightTransform, EntityDynamic* bird, std::vector<EntityStaticInstanced*> items, std::vector<EntityStaticInstanced*> trees, std::vector<EntityStatic*> lightSources, TerrainDiaSq* terrain);

    std::unique_ptr<Shader> m_dynamicShader;
    std::unique_ptr<Shader> m_instancingShader;
    std::unique_ptr<Shader> m_groundShader;
    std::unique_ptr<Shader> m_staticShader;
public:
    Shadow(LightContainer *lc, std::string settingPath);
    void renderDepthMap(EntityDynamic* bird, std::vector<EntityStaticInstanced*> items, std::vector<EntityStaticInstanced*> trees, std::vector<EntityStatic*> lightSources, TerrainDiaSq* terrain);
    
    ~Shadow();
};





#endif