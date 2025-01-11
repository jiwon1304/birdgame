#ifndef __LIGHTING_H__
#define __LIGHTING_H__

#include <utils/glPrerequisites.h>

#include <glad/glad.h>
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <learnopengl/shader_m.h>

class Light{    
public:
    Light() {}
    Light(glm::vec3 pos, glm::vec3 col, float amStr, float specStr)
        : position(pos), color(col), ambientStrength(amStr), specularStrength(specStr) {}
    virtual void setToShader(const Shader *shader) const {}
    
    void initGL(); // @todo: later
    // void draw(Shader *shader) const;

    glm::vec3 position;
    glm::vec3 color;
    float ambientStrength;
    float specularStrength;
    // float specularPower;
    GLuint VAO, VBO;
    // Cube *cube;

protected:
    glm::mat4 model;
};

class DirLight : public Light{
public:
    // position is just for light object render
    DirLight( glm::vec3 col, float amStr, float specStr, glm::vec3 dir)
        : Light(glm::vec3(0,0,0), col,amStr,specStr), direction(dir) { index = -1; }
    void setToShader(const Shader *shader) const;

    glm::vec3 direction;
    int index;
};

class PointLight : public Light{
public:
    PointLight(glm::vec3 pos, glm::vec3 col, float amStr, float specStr, float con, float lin, float quad)
        : Light(pos,col,amStr,specStr), constant(con), linear(lin), quadratic(quad) { index = -1; }
    void setToShader(const Shader *shader) const;

    float constant;
    float linear;
    float quadratic;
    int index;
};

class SpotLight : public Light{
public:

    SpotLight(glm::vec3 pos, glm::vec3 col, float amStr, float specStr, glm::vec3 dir, float co, float oco, float con, float lin, float quad)
        : Light(pos,col,amStr,specStr), direction(dir), cosInnerCutOff(co), cosOuterCutoff(oco), constant(con), linear(lin), quadratic(quad) { index = -1; }
    void setToShader(const Shader *shader) const;

    glm::vec3 direction;

    float cosInnerCutOff;
    float cosOuterCutoff;

    float constant;
    float linear;
    float quadratic;

    int index;
};



#endif