#ifndef __COLLISION_H__
#define __COLLISION_H__

#include <vector>
#include <src/entities/entity.h>
#include <src/terrain/terrain.hpp>

bool checkCollsionEntityRadius(const Entity* const A, const Entity* const B);
bool checkCollsionEntityRadiusInstanced(const Entity* const A, EntityStaticInstanced* B);
bool checkCollsionEntityAABB(const Entity* const A, const Entity* const B);
bool checkCollsionEntityAABB(const Entity* const A, const std::vector<Entity*> B);
bool checkCollisionEntityAABBInstanced(const Entity* const A, const EntityStaticInstanced* const B);


bool checkCollisionTerrain(const Entity* const A, const Terrain* const T);
// bool checkCollisionTerrainAABB(const Entity* const A, const Terrain* const T);

#endif