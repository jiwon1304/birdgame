#include "entity.h"
#include <utils/include/sphere.h>

Entity::Entity(string const &path){
    m_modelOffset = glm::mat4(1.0f);
    m_scale = glm::vec3(1,1,1);
    m_pitch = m_yaw = m_roll = 0;

    m_model = new Model(path);
    m_physics.m_position = m_physics.m_velocity = m_physics.m_velocity = glm::vec3(0,0,0);
}

// Entity::~Entity(){
//     delete m_model;
//     delete m_anim;
//     delete m_animator;
// }

// Entity::Entity(const Entity& other){
//     m_model = new Model(*other.m_model);
//     m_anim = new Animation(*m_anim);
//     m_animator = new Animator(*m_animator);
// }

// Entity& Entity::operator=(const Entity& other)
// {
//     if(this == &other) { return *this; }
//     delete m_model;
//     delete m_anim;
//     delete m_animator;
//     m_model = new Model(*other.m_model);
//     m_anim = new Animation(*m_anim);
//     m_animator = new Animator(*m_animator);
//     return *this;
// }

// void Entity::operator delete(void* ptr){
//     delete ((Entity*)ptr)->m_model;
//     delete ((Entity*)ptr)->m_anim;
//     delete ((Entity*)ptr)->m_animator;
//     delete (Entity*)ptr;
// }

/// @brief 모델 파일을 로드할 때의 오프셋 설정. 이후 모델은 항상 x축을 바라봄.
/// @param modelFront 모델 파일 내에서 모델이 바라보고 있는 방향
/// @param translationOffset 모델이 원점을 기준으로 어디에 위치할지 조정
/// @param scaleOffset 화면 내에서 평균적으로 등장할 크기로 조정
void Entity::setOffset(glm::vec3 modelFront, glm::vec3 translationOffset, glm::vec3 scaleOffset){
    m_modelOffset = glm::translate(glm::mat4(1.0f), translationOffset);
    // no need to rotate
    modelFront = glm::normalize(modelFront);
    if(modelFront != glm::vec3(1,0,0)){
        float rad = glm::acos(glm::dot(modelFront, glm::vec3(1,0,0)));
        glm::vec3 axis = glm::cross(modelFront, glm::vec3(1,0,0));
        m_modelOffset = glm::rotate(m_modelOffset, rad, axis);
    }
    m_modelOffset = glm::scale(m_modelOffset, scaleOffset);
}

/// @brief 모델이 바라보는방향 (m_front)이 x축, 위쪽이 y축, 오른쪽이 z축이되어 현위치 기준 이동합니다.
/// @param moveVector 
void Entity::move(glm::vec3 moveVector){
    glm::mat4 rot(glm::quat(glm::vec3(m_roll, m_yaw, m_pitch)));
    glm::vec3 front = rot * glm::vec4(1,0,0,0);
    glm::vec3 right = rot * glm::vec4(0,0,1,0);
    glm::vec3 up = rot * glm::vec4(0,1,0,0);

    glm::vec3 dx = moveVector.x * front;
    glm::vec3 dy = moveVector.y * up;
    glm::vec3 dz = moveVector.z * right;
    this->m_physics.m_position += dx + dy + dz;
}

void Entity::move(float front, float up, float right){
    this->move(glm::vec3(front, up, right));
}

// void Entity::setPosition(const glm::vec3 pos) { m_position = pos; }
void Entity::setScale(const glm::vec3 scale) { m_scale = scale; }
void Entity::setOrientation(glm::vec3 front, const float roll){
    front = glm::normalize(front);
    m_yaw = glm::atan(-front.z / front.x);
    m_pitch = glm::asin(front.y);
    if(front.x >= 0){
        if(m_yaw < 0) { m_yaw += 2*glm::pi<float>(); }
    }
    else{
        m_yaw += glm::pi<float>();
    }
    m_roll = roll;
}

glm::mat4 Entity::getOffsetMat() const { return m_modelOffset; }
glm::mat4 Entity::getOffsetTranslateMat() const { 
    glm::mat4 res(1.0f);
    res[3][0] = m_modelOffset[3][0];
    res[3][1] = m_modelOffset[3][1];
    res[3][2] = m_modelOffset[3][2];
    res[3][3] = m_modelOffset[3][3];

    return res;
}

void Entity::setPhysics(const glm::vec3 pos, const glm::vec3 vel, const glm::vec3 acc){
    this->m_physics = PhysicsProperties(pos, vel, acc);
}
void Entity::rotatePhysics(const glm::vec3 axis, const float degree){
    this->m_physics.rotateProperties(axis, degree);
}
void Entity::updatePhysics(const float deltaTime){
    this->m_physics.updateProperties(deltaTime);
}
void Entity::setPosition(const glm::vec3 pos){
    this->m_physics.m_position = pos;
}
void Entity::setVelocity(const glm::vec3 vel){
    this->m_physics.m_velocity = vel;
}
void Entity::setAcceleration(const glm::vec3 acc){
    this->m_physics.m_acceleration = acc;
}
void Entity::deltaPosition(const glm::vec3 deltaPos){
    this->m_physics.m_position += deltaPos;
}
void Entity::deltaVelocity(const glm::vec3 deltaVel){
    this->m_physics.m_velocity += deltaVel;
}
void Entity::deltaAcceleration(const glm::vec3 deltaAcc){
    this->m_physics.m_acceleration += deltaAcc;
}


glm::vec3 Entity::getPosition() const{
    return this->m_physics.m_position;
}

glm::vec3 Entity::getFront() const {
    float x = glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));
    float y = glm::sin(glm::radians(m_pitch));
    float z = glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch));

    return glm::vec3(x,y,z);
}
glm::vec3 Entity::getVelocity() const{
    return this->m_physics.m_velocity;
}
glm::vec3 Entity::getAcceleration() const{
    return this->m_physics.m_acceleration;
}

/// @brief translation, scale, front, up((0,1,0)이 아닐 수 있음) 기준으로 
void Entity::draw(Shader* shader){
    // this->updateAnimation(deltaTime, shader);
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, this->m_physics.m_position);

    // // this may has gimbal lock and not interpolation-able
    // // if(m_front != glm::vec3(1,0,0)){
    // //     float rad = glm::acos(glm::dot(glm::vec3(1,0,0), m_front));
    // //     glm::vec3 axis = glm::cross(glm::vec3(1,0,0), m_front);
    // //     model = glm::rotate(model, rad, axis);
    // // }
    
    // // we can expect that x-axis is the model's front (where "face" is heading)
    // // and y-axis is the model's up
    // // pitch(rotated angle with axis as z-axis) and yaw(... y-axis)
    // // what about roll? -> from up angle
    // // but how to compare with...? -> not rotated angle with x-axis
    // // roll is degree between P(itch)Y(aw)Rotated y-axis and m_up
    // // m_front and 

    glm::quat q(glm::vec3(m_roll, m_yaw, m_pitch));
    glm::mat4 rotation = glm::toMat4(q);
    model = model * rotation;

    model = glm::scale(model, m_scale);
    shader->use();
    shader->setMat4("model", model * m_modelOffset);

    // material (optional)
    shader->setVec3("material.color", this->materialColor);
    shader->setFloat("material.shininess", this->materialShininess);
    
    m_model->Draw(*shader);
}

// void Entity::bindLight(DirLight* light){
//     dirLight = light;
// }
// void Entity::bindLight(SpotLight* light){
//     spotLight = light;
// }
// void Entity::bindLight(PointLight* light){
//     pointLight = light;
// }
void Entity::setLightToShader(Shader* shader) const{
    if(dirLight != nullptr) { dirLight->setToShader(shader); }
    if(spotLight != nullptr) { spotLight->setToShader(shader); }
    if(pointLight != nullptr) { pointLight->setToShader(shader); }
}

void EntityStatic::draw(Shader* shader){
    glm::mat4 model(1.0f);
    model = glm::translate(model, this->m_physics.m_position);

    glm::quat q(glm::vec3(m_roll, m_yaw, m_pitch));
    glm::mat4 rotation = glm::toMat4(q);
    model = model * rotation;

    model = glm::scale(model, m_scale);
    shader->use();
    shader->setMat4("model", model * m_modelOffset);

    // material (optional)
    shader->setVec3("material.color", this->materialColor);
    shader->setFloat("material.shininess", this->materialShininess);
    
    // std::cerr << m_model->meshes[1].vertices[1].Position[0];
    m_model->Draw(*shader);
}

EntityDynamic::EntityDynamic(string const& path) : Entity(path){
    m_anim = new Animation(path, m_model);
    m_animator = new Animator(m_anim);
}

void EntityDynamic::setAnimationSpeed(const float speed){
    m_animationSpeed = speed;
}

void EntityDynamic::startAnimation(){
    m_animator->PlayAnimation(m_anim);
}

/// @brief translation, scale, front, up((0,1,0)이 아닐 수 있음) 기준으로 
void EntityDynamic::draw(Shader* shader){
    
    glm::mat4 model(1.0f);
    model = glm::translate(model, this->m_physics.m_position);

    // this may has gimbal lock and not interpolation-able
    // if(m_front != glm::vec3(1,0,0)){
    //     float rad = glm::acos(glm::dot(glm::vec3(1,0,0), m_front));
    //     glm::vec3 axis = glm::cross(glm::vec3(1,0,0), m_front);
    //     model = glm::rotate(model, rad, axis);
    // }
    
    // we can expect that x-axis is model's front (where "face" is heading)
    // and y-axis is model's up
    // pitch(rotated angle with axis as z-axis) and yaw(... y-axis)
    // what about roll? -> from up angle
    // but how to compare with...? -> not rotated angle with x-axis
    // roll is degree between P(itch)Y(aw)Rotated y-axis and m_up
    // m_front and 

    glm::quat q(glm::vec3(m_roll, m_yaw, m_pitch));
    glm::mat4 rotation = glm::toMat4(q);
    model = model * rotation;

    model = glm::scale(model, m_scale);
    shader->use();
    shader->setMat4("model", model * m_modelOffset);

    // material (optional)
    shader->setVec3("material.color", this->materialColor);
    shader->setFloat("material.shininess", this->materialShininess);

    auto transforms = m_animator->GetFinalBoneMatrices();
    for(int i =0; i < transforms.size(); i++){
        shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }
    
    m_model->Draw(*shader);
}

void EntityDynamic::updateAnimation(float deltaTime){
    // shader->use();
    const float min_velocity = 5.0f;
    float animSpeed = glm::min(5.0*(this->m_physics.m_velocity.y-min_velocity)+min_velocity, (double)20.0);
    this->setAnimationSpeed(this->m_physics.m_velocity.y > min_velocity?animSpeed:min_velocity);

    m_animator->UpdateAnimation(m_animationSpeed * deltaTime);
    // auto transforms = m_animator->GetFinalBoneMatrices();
    // for(int i =0; i < transforms.size(); i++){
    //     shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    // }
}

void EntityStaticInstanced::draw(Shader* shader){
    shader->use();
    for(int i = 0; i < this->modelMatInstanced.size(); i++){
        glm::mat4 model(1.0f);
        model = modelMatInstanced[i];
        glm::quat q(glm::vec3(m_roll, m_yaw, m_pitch));
        glm::mat4 rotation = glm::toMat4(q);
        model = model * rotation;

        model = glm::scale(model, m_scale);

        shader->setMat4("model[" + std::to_string(i) + "]", model * m_modelOffset);
    }

    // material (optional)
    shader->setVec3("material.color", this->materialColor);
    shader->setFloat("material.shininess", this->materialShininess);
    
    // std::cerr << m_model->meshes[1].vertices[1].Position[0];
    m_modelInstancing->Draw(*shader, this->modelMatInstanced.size());
}
