#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

// uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    vec4 modelVec4 = vec4(aPos, 1.0f);
    gl_Position = projection * view * modelVec4;
    vs_out.FragPos = vec3(modelVec4);
    vs_out.Normal = aNormal;
    // vs_out.Normal = vec3(transpose(inverse(mat3(model))) * aNormal);
    vs_out.TexCoords = vec2(aPos.x, aPos.z) / 10.0;
}
