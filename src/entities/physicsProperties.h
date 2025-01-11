#ifndef __PHYSICS_PROPERTIES_H__
#define __PHYSICS_PROPERTIES_H__

// #include <utils/glPrerequisites.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PhysicsProperties{
public:
    PhysicsProperties(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc);
    PhysicsProperties();

    // void setPosition(const glm::vec3 pos);
    // void setVelocity(const glm::vec3 vel);
    // void setAcceleration(const glm::vec3 acc);

    void rotateProperties(const glm::vec3 axis, const float degree);

    void updateProperties(const float deltaTime);

    // glm::vec3 getPosition() const;
    // glm::vec3 getVelocity() const;
    // glm::vec3 getAcceleration() const;

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
};

#endif