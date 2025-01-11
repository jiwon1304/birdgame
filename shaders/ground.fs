#version 330 core

// TODO : duplicated calculations in each calc function
// normal, ambient, ...

#define NUM_DIRLIGHTS 3
#define NUM_POINTLIGHTS 0
#define NUM_SPOTLIGHTS 0

// can be modified from class Entity
struct Material{
    vec3 color;
    float shininess;
};

struct DirLight{
    vec3 color;
    float ambientStrength;
    float specularStrength;

    vec3 direction;
};

struct PointLight{
    vec3 color;
    float ambientStrength;
    float specularStrength;
    
    vec3 position;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight{
    vec3 color;
    float ambientStrength;
    float specularStrength;
    
    vec3 position;
    vec3 direction;
    float cosInnerCutOff; // cosine values
    float cosOuterCutOff;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

// in vec2 TexCoords;
// in vec3 FragPos;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;


uniform vec3 viewPos;
uniform Material material;
uniform sampler2D texture_diffuse1;

#if NUM_DIRLIGHTS > 0
uniform DirLight dirLights[NUM_DIRLIGHTS];
#endif

#if NUM_POINTLIGHTS > 0
uniform PointLight pointLights[NUM_POINTLIGHTS];
#endif

#if NUM_SPOTLIGHTS > 0
uniform SpotLight spotLights[NUM_SPOTLIGHTS];
#endif


// view = viewpos - fragpos, all vectors not in light class should be normalized
vec3 CalcDirLightBlinn(DirLight light, vec3 normal, vec3 view);
vec3 CalcPointLightBlinn(PointLight light, vec3 normal, vec3 fragPos, vec3 view);
vec3 CalcSpotLightBlinn(SpotLight light, vec3 normal, vec3 fragPos, vec3 view);

void main(){
    // direction normalized, positions not normalized
    vec3 norm = normalize(vs_out.Normal);
    vec3 frag = vs_out.FragPos;
    // frag -> view
    vec3 view = normalize(viewPos - vs_out.FragPos);

    vec3 result = vec3(0,0,0);

#if NUM_DIRLIGHTS > 0
    for(int i = 0; i < NUM_DIRLIGHTS; i++)
        result += CalcDirLightBlinn(dirLights[i], norm, view);
#endif
#if NUM_POINTLIGHTS > 0
    for(int i = 0; i < NUM_POINTLIGHTS; i++)
        result += CalcPointLightBlinn(pointLights[i], norm, frag, view);
#endif
#if NUM_POINTLIGHTS > 0
    for(int i = 0; i < NUM_SPOTLIGHTS; i++)
        result += CalcSpotLightBlinn(spotLights[i], norm, frag, view);
#endif
    FragColor = vec4(result, 1.0f);
    FragColor += vec4(material.color, 1.f);
}

vec3 CalcDirLightBlinn(DirLight light, vec3 normal, vec3 viewDir){
    vec3 ambientVec3 = light.color * light.ambientStrength * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    
    vec3 lightDir = normalize(-light.direction);
    // return vs_out.Normal;

    float diffuse = max(dot(normal, lightDir), 0.0f);
    if(diffuse == 0) // frag is not lit
        return ambientVec3;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular;
    if(material.shininess <= 0){
        specular = 0;
    }
    else{
        specular = pow(max(dot(halfwayDir, normal), 0.0f), material.shininess);
    }
    // float specular = 0;
        
    vec3 diffuseVec3 = diffuse * light.color * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    vec3 specularVec3 = specular * light.color * light.specularStrength * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));

    // return (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    return ambientVec3 + diffuseVec3 + specularVec3;
}

vec3 CalcPointLightBlinn(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 ambientVec3 = light.color * light.ambientStrength * 
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
    vec3 lightDir = normalize(light.position - fragPos);

    float diffuse = max(dot(normal, lightDir), 0.0f);
    if(diffuse == 0) // frag is not lit
        return ambientVec3;
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular;
    if(material.shininess <= 0){
        specular = 0;
    }
    else{
        specular = pow(max(dot(halfwayDir, normal), 0.0f), material.shininess);
    }
    // float specular = 0;


    vec3 diffuseVec3 = diffuse * light.color * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    vec3 specularVec3 = specular * light.color * light.specularStrength * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    
    return attenuation * (ambientVec3 + diffuseVec3 + specularVec3);
}

vec3 CalcSpotLightBlinn(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 ambientVec3 = light.color * light.ambientStrength * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    vec3 lightDir = normalize(light.position - fragPos);

    float diffuse = max(dot(normal, lightDir), 0.0f);
    if(diffuse == 0) // frag is not lit
        return ambientVec3;
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // intensity : degrees are actully cosine value
    float theta = dot(normalize(light.direction), -lightDir);
    // degree 0 ~ cutoff : 1
    // cutoff ~ outercutoff : 1~0
    // cutoff ~ : 0
    // note that all values are cosine values; more degree means less value
    float intensity = clamp((theta - light.cosOuterCutOff) / (light.cosInnerCutOff - light.cosOuterCutOff), 0.0f, 1.0f);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float specular;
    if(material.shininess <= 0){
        specular = 0;
    }
    else{
        specular = pow(max(dot(halfwayDir, normal), 0.0f), material.shininess);
    }
    // float specular = 0;

    
    vec3 diffuseVec3 = diffuse * light.color * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));
    vec3 specularVec3 = specular * light.color * light.specularStrength * 
        // (vec3(texture(texture_diffuse1, vs_out.TexCoords)) + material.color);
        (vec3(texture(texture_diffuse1, vs_out.TexCoords)));

    // return dot(halfwayDir, normal) * vec3(0,1,0);
    // return max(dot(halfwayDir, normal), 0.0f) * vec3(0,1,0);
    //  * vec3(1,1,1);
    // return diffuse * light.diffuse;
    // return intensity * attenuation * ambientVec3;
    return intensity * attenuation * (ambientVec3 + diffuseVec3 + specularVec3);
}