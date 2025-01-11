// #include "entity_light.h"

// EntityLight::EntityLight(string const &path, DirLight* const d) : Entity(path), type(DIRLIGHT) {
//     this->dirLight = d;   
// }
// EntityLight::EntityLight(string const &path, PointLight* const p) : Entity(path), type(POINTLIGHT) {
//     this->pointLight = p;   
// }
// EntityLight::EntityLight(string const &path, SpotLight* const s) : Entity(path), type(SPOTLIGHT) {
//     this->spotLight = s;   
// }

// DirLight* const EntityLight::getDirLight() const{
//     assert(type == LightType::DIRLIGHT);
//     return this->dirLight;
// }
// PointLight* const EntityLight::getPointLight() const{
//     assert(type == LightType::POINTLIGHT);
//     return this->pointLight;
// }
// SpotLight* const EntityLight::getSpotLight() const{
//     assert(type == LightType::DIRLIGHT);
//     return this->spotLight;
// }


// // EntityDirLight::EntityDirLight(string const& path, DirLight* _light)
// //     : Entity(path), light(_light){
// // }

// // void EntityDirLight::setToShader(const Shader* shader) const{
// //     light->setToShader(shader);
// // }

// // EntityPointLight::EntityPointLight(string const& path, PointLight* _light)
// //     : Entity(path), light(_light){
// // }

// // void EntityPointLight::setToShader(const Shader* shader) const{
// //     light->setToShader(shader);
// // }

// // EntitySpotLight::EntitySpotLight(string const& path, SpotLight* _light)
// //     : Entity(path), light(_light){
// // }

// // void EntitySpotLight::setToShader(const Shader* shader) const{
// //     light->setToShader(shader);
// // }
