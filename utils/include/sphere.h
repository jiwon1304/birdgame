#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * Usage : in render call (which is called in the render loop)
 * 
 * static Sphere sphere(radius);
 * sphere.draw();
 * 
 * you can change the radius
 **/
class Sphere
{
private:
    unsigned int m_VAO;
    unsigned int m_VBO;
    GLuint ID;
    inline void init();
    inline void checkCompileErrorsSphere(GLuint shader, std::string type);
public:
    Sphere(float radius) : m_radius(radius) { this->init(); };
    inline void draw(const glm::mat4 model, const glm::mat4 view, const glm::mat4 proj);
    float SphereVertex[3*21*6];
    float m_radius;
};

void Sphere::init()
{   
    // init sphere vertices
    for(int i=0; i < 16 + 4 + 1; i++){
        SphereVertex[i*6 + 0] = 0;
        SphereVertex[i*6 + 1] = glm::cos(2 * glm::pi<float>() * i / 16.f);
        SphereVertex[i*6 + 2] = glm::sin(2 * glm::pi<float>() * i / 16.f);
        SphereVertex[i*6 + 3] = 1.f;
        SphereVertex[i*6 + 4] = 0.f;
        SphereVertex[i*6 + 5] = 0.f;
    }
    for(int i=0; i < 16 + 4 + 1; i++){
        SphereVertex[21*6 + i*6 + 0] = glm::sin(2 * glm::pi<float>() * i / 16.f);
        SphereVertex[21*6 + i*6 + 1] = 0;
        SphereVertex[21*6 + i*6 + 2] = glm::cos(2 * glm::pi<float>() * i / 16.f);
        SphereVertex[21*6 + i*6 + 3] = 0.f;
        SphereVertex[21*6 + i*6 + 4] = 1.f;
        SphereVertex[21*6 + i*6 + 5] = 0.f;
    }
    for(int i=0; i < 16 + 4 + 1; i++){
        SphereVertex[21*2*6 + i*6 + 0] = glm::cos(2 * glm::pi<float>() * i / 16.f);
        SphereVertex[21*2*6 + i*6 + 1] = glm::sin(2 * glm::pi<float>() * i / 16.f);
        SphereVertex[21*2*6 + i*6 + 2] = 0;
        SphereVertex[21*2*6 + i*6 + 3] = 0.f;
        SphereVertex[21*2*6 + i*6 + 4] = 0.f;
        SphereVertex[21*2*6 + i*6 + 5] = 1.f;
    }

    const char *vShaderCode = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "uniform float radius;\n"
    "out vec3 color;\n"
    "void main()\n"
    "{\n"
    "   color = aColor;\n"
    "   gl_Position = projection*view*model*vec4(radius*aPos,1.0f);\n"
    "}\0";
    const char *fShaderCode = "#version 330 core\n"
    "in vec3 color;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color, 1.0f);\n"
    "}\0";
    // generate shader 

    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrorsSphere(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrorsSphere(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrorsSphere(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    // copy vertex attrib data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SphereVertex), SphereVertex, GL_STATIC_DRAW); // reserve space
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));	// Vertex attributes stay the same
    glEnableVertexAttribArray(1);
}

void Sphere::draw(const glm::mat4 model, const glm::mat4 view, const glm::mat4 proj){
    glUseProgram(ID);
    glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform1f(glGetUniformLocation(ID, "radius"), m_radius);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINE_STRIP, 0, 3*21);
}

void Sphere::checkCompileErrorsSphere(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


#endif