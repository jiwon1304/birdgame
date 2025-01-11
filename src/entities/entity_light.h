// #ifndef __ENTITY_LIGHT_H__
// #define __ENTITY_LIGHT_H__

// #include "entity.h"
// #include <src/lighting/lighting.h>

// enum LightType{
//     DIRLIGHT, POINTLIGHT, SPOTLIGHT
// };

// class EntityLight : public Entity{
// public:
//     EntityLight(string const &path, DirLight* const d);
//     EntityLight(string const &path, PointLight* const p);
//     EntityLight(string const &path, SpotLight* const s);

//     DirLight* const getDirLight() const;
//     PointLight* const getPointLight() const;
//     SpotLight* const getSpotLight() const;
// private:
//     LightType type;
// };



// // class EntityDirLight : public Entity{
// // public:
// //     EntityDirLight(string const &path, DirLight* _light);
// // private:
// //     void setToShader(const Shader *shader) const;
// //     DirLight* light;
// // };

// // class EntityPointLight : public Entity{
// // public:
// //     EntityPointLight(string const &path, PointLight* _light);

// // private:
// //     void setToShader(const Shader *shader) const;
// //     PointLight* light;
// // };

// // class EntitySpotLight : public Entity{
// // public:
// //     EntitySpotLight(string const &path, SpotLight* _light);
// //     void setToShader(const Shader *shader) const;

// // private:
// //     SpotLight* light;
// // };

// #endif