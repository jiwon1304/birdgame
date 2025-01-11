#include "physicsProperties.h"

PhysicsProperties::PhysicsProperties(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc){
    m_position = pos;
    m_velocity = vel;
    m_acceleration = acc;
}

PhysicsProperties::PhysicsProperties(){
    m_position = glm::vec3(0.f, 0.f, 0.f);
    m_velocity = glm::vec3(0.f, 0.f, 0.f);
    m_acceleration = glm::vec3(0.f, 0.f, 0.f);
}


// inline void PhysicsProperties::setPosition(const glm::vec3 pos){
//     m_position = glm::vec4(pos, 1.f);
// }
// inline void PhysicsProperties::setVelocity(const glm::vec3 vel){
//     m_velocity = glm::vec4(vel, 0.f);
// }
// inline void PhysicsProperties::setAcceleration(const glm::vec3 acc){
//     m_acceleration = glm::vec4(acc, 0.f);
// }

void PhysicsProperties::rotateProperties(const glm::vec3 axis, const float degree){
    glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(degree), axis);
    m_position = glm::vec3(rot * glm::vec4(m_position, 1.f));
    m_velocity = glm::vec3(rot * glm::vec4(m_velocity, 0.f));
    m_acceleration = glm::vec3(rot * glm::vec4(m_acceleration, 0.f));
}

void PhysicsProperties::updateProperties(const float deltaTime){
    // euler method
    m_velocity += m_acceleration * deltaTime;
    m_position += m_velocity * deltaTime;
}

// inline glm::vec3 PhysicsProperties::getPosition() const{
//     return glm::vec3(m_position);
// }
// inline glm::vec3 PhysicsProperties::getVelocity() const{
//     return glm::vec3(m_velocity);
// }
// inline glm::vec3 PhysicsProperties::getAcceleration() const{
//     return glm::vec3(m_acceleration);
// }
