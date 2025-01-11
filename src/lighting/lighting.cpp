#include "lighting.h"

// void Light::initGL(){
//     cube = new Cube();
// }

// void Light::draw(Shader *shader) const{
//     shader->setMat4("model", glm::translate(glm::mat4(1.0f), this->position));
//     cube->draw(shader);
// }

void DirLight::setToShader(const Shader *shader) const{
    shader->use();
    assert(this->index != -1 && "dirLight has no index");
    // std::cout << this->index << std::endl;
    std::string prefix = "dirLights[" + std::to_string(this->index) + "].";
    shader->setVec3(prefix + "color", color);
    shader->setFloat(prefix + "ambientStrength", ambientStrength);
    shader->setFloat(prefix + "specularStrength", specularStrength);
    shader->setVec3(prefix + "direction", direction);
    // shader->setFloat("specularPower", specularPower);
}

void PointLight::setToShader(const Shader *shader) const{
    assert(this->index != -1 && "pointLight has no index");
    std::string prefix = "pointLights[" + std::to_string(this->index) + "].";
    shader->use();
    shader->setVec3(prefix + "lightColor", color);
    shader->setFloat(prefix + "ambientStrength", ambientStrength);
    shader->setFloat(prefix + "specularStrength", specularStrength);
    // shader->setFloat("specularPower", specularPower);

    shader->setVec3(prefix + "position", position);

    shader->setFloat(prefix + "constant", constant);
    shader->setFloat(prefix + "linear", linear);
    shader->setFloat(prefix + "quadratic", quadratic);
}

void SpotLight::setToShader(const Shader *shader) const{
    shader->use();
    assert(this->index != -1 && "spotLight has no index");
    std::string prefix = "spotLights[" + std::to_string(this->index) + "].";

    shader->setVec3(prefix + "lightColor", color);
    shader->setFloat(prefix + "ambientStrength", ambientStrength);
    shader->setFloat(prefix + "specularStrength", specularStrength);
    // shader->setFloat("specularPower", specularPower);

    shader->setVec3(prefix + "position", position);
    shader->setVec3(prefix + "direction", direction);
    shader->setFloat(prefix + "cosInnerCutOff", cosInnerCutOff);
    shader->setFloat(prefix + "cosOuterCutoff", cosOuterCutoff);

    shader->setFloat(prefix + "constant", constant);
    shader->setFloat(prefix + "linear", linear);
    shader->setFloat(prefix + "quadratic", quadratic);
}
