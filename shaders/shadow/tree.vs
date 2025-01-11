// #version 330 core
// layout ( location = 0 ) in vec3 position;
// layout ( location = 1 ) in vec3 normal;
// layout ( location = 2 ) in vec2 texCoords;

// out vec2 TexCoords;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

// out VS_OUT{
//     vec3 FragPos;
//     vec3 Normal;
//     vec2 TexCoords;
// } vs_out;

// void main( )
// {
//     vec4 modelVec4 = model * vec4(position, 1.0f);
//     gl_Position = projection * view * modelVec4;
//     vs_out.FragPos = vec3(modelVec4);
//     vs_out.Normal = vec3(transpose(inverse(mat3(model))) * normal);
//     vs_out.TexCoords = texCoords;
    
// }


#version 330 core
layout ( location = 0 ) in vec3 position;
// layout ( location = 1 ) in vec3 normal;
// layout ( location = 2 ) in vec2 texCoords;`

// out vec2 TexCoords;

#define NUM_TREES 200

uniform mat4 model[NUM_TREES];
// uniform mat4 view;
// uniform mat4 projection;
uniform mat4 lightTransform;


// out VS_OUT{
//     vec3 FragPos;
//     vec3 Normal;
//     vec2 TexCoords;
// } vs_out;

void main( )
{
    vec4 modelVec4 = model[gl_InstanceID] * vec4(position, 1.0f);
    gl_Position = lightTransform * modelVec4;
    // vs_out.FragPos = vec3(modelVec4);
    // vs_out.Normal = vec3(transpose(inverse(mat3(model[gl_InstanceID]))) * normal);
    // vs_out.TexCoords = texCoords;
    
}
