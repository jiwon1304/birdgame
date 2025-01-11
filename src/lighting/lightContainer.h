#ifndef __LIGHTING_CONTAINER_H__
#define __LIGHTING_CONTAINER_H__
#include <vector>
#include "lighting.h"

// container for light objects
// only for rendering
// modification of light object is job of entity class
class LightContainer{
public:
    // light argument should be
    void add(DirLight* light);
    void add(PointLight* light);
    void add(SpotLight* light);

    void remove(DirLight* light);
    void remove(PointLight* light);
    void remove(SpotLight* light);

    glm::ivec3 getNum() const;
    // cache위해서 포인터로?
    const std::vector<DirLight*>* getDirLight() const;
    const std::vector<PointLight*>* getPointLight() const;
    const std::vector<SpotLight*>* getSpotLight() const;

    void setToShader(const Shader* const shader);

    // util
    void printNum();
private:
    std::vector<DirLight*> dirLights;
    std::vector<PointLight*> pointLights;
    std::vector<SpotLight*> spotLights;
};
#endif