#ifndef __GAME_IO_H__
#define __GAME_IO_H__

#include <utils/glPrerequisites.h>
#include <utils/learnopenglPrerequisites.h>
#include <src/entities/entity.h>

class GameIO{
public:
    GameIO(GLFWwindow* w, Camera *c);
    GameIO() {}
    void init();
    void bindEntity(Entity *e);
    void poll(float deltaTime);
private:
    GLFWwindow *window;
    Camera *camera;
    Entity* entity;
    void pollCameraIO(float deltaTime);
    void pollEntityIO(float deltaTime);

};

#endif