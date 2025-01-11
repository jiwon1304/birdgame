#version 330 core
layout (location = 0) in vec3 aPos;

// uniform mat4 model;
uniform mat4 lightTransform;

void main(){
    vec4 modelVec4 = vec4(aPos, 1.0f);
    gl_Position = lightTransform * modelVec4;
}
