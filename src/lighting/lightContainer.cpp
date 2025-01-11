#include "lightContainer.h"
#include <iostream>

void LightContainer::add(DirLight* const light){
    light->index = dirLights.size();
    this->dirLights.push_back(light);
}

void LightContainer::add(PointLight* const light){
    light->index = pointLights.size();
    this->pointLights.push_back(light);
}

void LightContainer::add(SpotLight* const light){
    light->index = spotLights.size();
    this->spotLights.push_back(light);
}

void LightContainer::remove(DirLight* const light){
    this->dirLights.erase(this->dirLights.begin() + light->index);
    light->index = -1;
}

void LightContainer::remove(PointLight* const light){
    this->pointLights.erase(this->pointLights.begin() + light->index);
    light->index = -1;
}

void LightContainer::remove(SpotLight* const light){
    this->spotLights.erase(this->spotLights.begin() + light->index);
    light->index = -1;
}

glm::ivec3 LightContainer::getNum() const
{
    return glm::ivec3(dirLights.size(), pointLights.size(), spotLights.size());
}

const std::vector<DirLight*>* LightContainer::getDirLight() const
{
    return &this->dirLights;
}

const std::vector<PointLight*>* LightContainer::getPointLight() const
{
    return &this->pointLights;
}

const std::vector<SpotLight*>* LightContainer::getSpotLight() const
{
    return &this->spotLights;
}

void LightContainer::setToShader(const Shader* const shader){
    for(auto i: this->dirLights){ i->setToShader(shader); }
    for(auto i: this->pointLights){ i->setToShader(shader); }
    for(auto i: this->spotLights){ i->setToShader(shader); }
}

void LightContainer::printNum(){
    std::cout << "size of dirLights :\t" <<dirLights.size() << std::endl; 
    std::cout << "size of pointLights :\t" <<pointLights.size() << std::endl; 
    std::cout << "size of spotLights :\t" <<spotLights.size() << std::endl; 
}

