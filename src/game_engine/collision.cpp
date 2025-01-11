#include "collision.h"

bool checkCollsionEntityRadius(const Entity* const A, const Entity* const B){
    float distance = glm::length(A->getPosition() - B->getPosition());
    float collisionRange = A->collisionRadius + B->collisionRadius;
    // std::cout << distance << std::endl << collisionRange << (distance < collisionRange) << std::endl << std::endl;
    return (distance < collisionRange);
}

bool checkCollsionEntityRadiusInstanced(const Entity* const A, EntityStaticInstanced* B)
{
    for(auto &M: B->modelMatInstanced){
        glm::vec3 pos = glm::vec3(M * glm::vec4(0,0,0,1));
        float distance = glm::distance(A->getPosition(), pos);

        float collisionRange = A->collisionRadius + B->collisionRadius;

        if(distance < collisionRange) { 
            M = glm::translate(glm::mat4(1.f), glm::vec3(0, -30, 0));
            return true;
        }
    }
    return false;
}

bool checkCollsionEntityAABB(const Entity* const A, const Entity* const B){
    glm::vec3 distance = glm::abs(A->getPosition() - B->getPosition());
    float dx = distance.x;
    float dy = distance.y;
    float dz = distance.z;

    float cx = A->collisionX + B->collisionX;
    float cy = A->collisionY + B->collisionY;
    float cz = A->collisionZ + B->collisionZ;

    std::cout << "dx :" + std::to_string(dx) + " dy: " + std::to_string(dy) + " dz: " + std::to_string(dz) << std::endl;
    std::cout << "cx :" + std::to_string(cx) + " cy: " + std::to_string(cy) + " cz: " + std::to_string(cz) << std::endl;
    if(cx > dx && cy > dy && cz > dz) { return true; }
    else { return false; }
}

bool checkCollsionEntityAABB(const Entity* const A, const std::vector<Entity*> B)
{
    for(const auto e: B){
        if(checkCollsionEntityAABB(A, e) == true){
            return true;
        }
    }
    return false;
}

bool checkCollisionEntityAABBInstanced(const Entity* const A, const EntityStaticInstanced* const B)
{
    for(const auto M: B->modelMatInstanced){
        glm::vec3 pos = glm::vec3(M * glm::vec4(0,0,0,1));
        glm::vec3 distance = glm::abs(A->getPosition() - pos);

        float dx = distance.x;
        float dy = distance.y;
        float dz = distance.z;

        float cx = A->collisionX + B->collisionX;
        float cy = A->collisionY + B->collisionY;
        float cz = A->collisionZ + B->collisionZ;

        if(cx > dx && cy > dy && cz > dz) { return true; }
    }

    return false;
}


bool checkCollisionTerrain(const Entity* const A, const Terrain* const T)
{
    // glm::ivec2 coord = glm::ivec2(int(A->getPosition().x), int(A->getPosition().z));
    glm::vec2 coord = glm::vec2(A->getPosition().x, A->getPosition().z);

    if(coord.x > 2 && coord.x < T->getSizeX() - 2 && coord.y > 2 && coord.y < T->getSizeZ() - 2){

        int fx = floor(coord.x);
        int fy = floor(coord.y);
        int cx = ceil(coord.x);
        int cy = ceil(coord.y);

        float h1 = T->getHeight(fx, fy);
        float h2 = T->getHeight(fx, cy);
        float h3 = T->getHeight(cx, fy);
        float h4 = T->getHeight(cx, cy);
        float averageHeight = (h1+h2+h3+h4) / 4.f;

        float distance = A->getPosition().y - averageHeight;
        if(distance < A->collisionRadius - 1){
            return true;
        }
    }

    return false;
}

// bool checkCollisionTerrainAABB(const Entity* const A, const Terrain* const T)
// {
//     return false;
// }
