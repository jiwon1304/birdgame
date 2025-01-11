#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <learnopengl/shader_m.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>

#include <src/lighting/lighting.h>
#include <glm/gtx/quaternion.hpp>
#include <vector>

#include "physicsProperties.h"
#include "instancing/modelInstancing.h"

class Entity{
public:
    Entity(string const &path);

    // offset(translation, rotation, scale...) of model file
    void setOffset(glm::vec3 modelFront, glm::vec3 translationOffset, glm::vec3 scaleOffset);

    void move(glm::vec3 moveVector);
    void move(float front, float up, float right);

    // void setPosition(const glm::vec3 pos);
    void setScale(const glm::vec3 scale);
    void setOrientation(glm::vec3 front, const float roll);

    glm::mat4 getOffsetMat() const;
    glm::mat4 getOffsetTranslateMat() const;

    // where model is heading to (compensated with m_modelFront)
    virtual void draw(Shader* shader);

    // just for rendering: change optional
    float materialShininess = 0;
    glm::vec3 materialColor = glm::vec3(0.f);
    
    // for physics
    void setPhysics(const glm::vec3 pos, const glm::vec3 vel, const glm::vec3 acc);
    void rotatePhysics(const glm::vec3 axis, const float degree);
    void updatePhysics(const float deltaTime);
    void setPosition(const glm::vec3 pos);
    void setVelocity(const glm::vec3 vel);
    void setAcceleration(const glm::vec3 acc);

    void deltaPosition(const glm::vec3 deltaPos);
    void deltaVelocity(const glm::vec3 deltaVel);
    void deltaAcceleration(const glm::vec3 deltaAcc);

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getVelocity() const;
    glm::vec3 getAcceleration() const;

    // for collision
    float collisionRadius = 1.f;
    float collisionX = 0.f;
    float collisionY = 0.f;
    float collisionZ = 0.f;

    // for moving
    float jumpVel = 1.f;

    // optional: bind light and set light to shader
    void setLightToShader(Shader* shader) const;
    // void bindLight(DirLight* light);
    // void bindLight(SpotLight* light);
    // void bindLight(PointLight* light);
    
    // optional : lights
    DirLight* dirLight;
    PointLight* pointLight;
    SpotLight* spotLight;
protected:
    Model *m_model;

    float m_stopTime = 0;
    glm::mat4 m_modelOffset;

    // glm::vec3 m_position;
    glm::vec3 m_scale;

    float m_pitch, m_yaw, m_roll;

    // physics
    PhysicsProperties m_physics;

    // animations
    float m_animationSpeed = 1.0f;


};

/**
 * 
 * TODO: Entity constructor은 애니메이션 빼기
 * derived constructor 만ㄷ르기
*/
// has no animation -> nothing happens at animation
class EntityStatic : public Entity{
public:
    EntityStatic(string const &path) : Entity(path) {}
    void draw(Shader* shader) override;
};

class EntityStaticInstanced : public EntityStatic{
public:
    EntityStaticInstanced(string const &path) : EntityStatic(path) {
        delete m_model;
        m_modelInstancing = new ModelInstancing(path);
    }
    std::vector<glm::mat4> modelMatInstanced;
    void draw(Shader* shader) override;
    ModelInstancing *m_modelInstancing;
};

class EntityDynamic : public Entity{
public:
    EntityDynamic(string const &path);
    void setAnimationSpeed(const float speed);
    void startAnimation();
    void updateAnimation(float deltaTime);
    void draw(Shader* shader) override;

private:
    Animation *m_anim;
    Animator *m_animator;
};
#endif