#include "shadow.h"
#include <utils/include/iniparser.hpp>

#define SHADOW_MAP_SIZE 1024
const std::string transformName = "lightTransform";

void Shadow::draw(glm::mat4 lightTransform, EntityDynamic* bird, std::vector<EntityStaticInstanced*> items, std::vector<EntityStaticInstanced*> trees, std::vector<EntityStatic*> lightSources, TerrainDiaSq* terrain){
    // draw
    if(bird != NULL) {
        bird->draw(m_dynamicShader.get());
    }

    // STATICS : INSTANCING
    m_instancingShader->use();
    // lightContainer.setToShader(instancingShader);

    m_instancingShader->setMat4(transformName, lightTransform);

    for(auto i : items){    
        i->draw(m_instancingShader.get());
    }
    for(auto t : trees){
        t->draw(m_instancingShader.get());
    }

    // GROUND
    m_groundShader->use();
    m_groundShader->setMat4(transformName, lightTransform);

    if(terrain != nullptr) { terrain->Draw(m_groundShader.get()); }

    // STATICS : NO INSTANCING
    m_staticShader->use();
    m_staticShader->setMat4(transformName, lightTransform);

    // glDepthMask(GL_FALSE);
    for(auto l : lightSources){
        l->draw(m_staticShader.get());
    }

}

Shadow::Shadow(LightContainer *lc, std::string settingPath){
    m_lightContainer = lc;
    m_numLights = lc->getNum();
    m_numDepthMaps = m_numLights.x + m_numLights.y + m_numLights.z;
    std::cerr << "NUMDEPTHMAP" <<  m_numDepthMaps;

    m_depthMapTextures = new unsigned int[m_numDepthMaps];
    m_depthMapFrames = new unsigned int[m_numDepthMaps];

    glGenFramebuffers(m_numDepthMaps, m_depthMapFrames);

    glGenTextures(m_numDepthMaps, m_depthMapTextures);
    for(int i=0; i<m_numDepthMaps; i++){
        glBindTexture(GL_TEXTURE_2D, m_depthMapTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                    SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);  

        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFrames[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMapTextures[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    }


    // init shader
    INI::File ft;
	if (!ft.Load(settingPath))
	{
		// Loading from stream
		std::ifstream op("opts.ini",std::ios::in);
		if (op.is_open())
			op >> ft;
	}

    const char fragPath[] = "SHADER_SHADOW_FRAG";
    std::cerr << fragPath << ft.GetValue("SHADER_SHADOW_DYNAMIC_VERT").AsString().c_str();
    m_dynamicShader = std::make_unique<Shader>(ft.GetValue("SHADER_SHADOW_DYNAMIC_VERT").AsString().c_str(), ft.GetValue(fragPath).AsString().c_str());
    m_instancingShader = std::make_unique<Shader>(ft.GetValue("SHADER_SHADOW_TREE_VERT").AsString().c_str(), ft.GetValue(fragPath).AsString().c_str());
    m_groundShader = std::make_unique<Shader>(ft.GetValue("SHADER_SHADOW_TERRAIN_VERT").AsString().c_str(), ft.GetValue(fragPath).AsString().c_str());
    m_staticShader = std::make_unique<Shader>(ft.GetValue("SHADER_SHADOW_STATIC_VERT").AsString().c_str(), ft.GetValue(fragPath).AsString().c_str());

    m_near = 0.1f;
    m_far = 500.f;
}

void Shadow::renderDepthMap(EntityDynamic* bird, std::vector<EntityStaticInstanced*> items, std::vector<EntityStaticInstanced*> trees, std::vector<EntityStatic*> lightSources, TerrainDiaSq* terrain){
    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    int idx = 0;
    // dirlight
    for(int i=0; i < m_numLights.x; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFrames[idx]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, m_near, m_far);
        DirLight *light = (*m_lightContainer->getDirLight())[i];
        glm::mat4 lightView = glm::lookAt(light->position, light->position + light->direction, glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 lightTransform = lightProjection * lightView;

        this->draw(lightTransform, bird, items, trees, lightSources, terrain);
    }
    // pointlight
    for(int i=0; i < m_numLights.y; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFrames[idx]);
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, m_near, m_far);
        DirLight *light = (*m_lightContainer->getDirLight())[i];
        glm::mat4 lightView = glm::lookAt(light->position, light->position + light->direction, glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 lightTransform = lightProjection * lightView;

        this->draw(lightTransform, bird, items, trees, lightSources, terrain);

    }
    // spotlight
    for(int i=0; i < m_numLights.z; i++){
        // glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFrames[idx]);
        // glClear(GL_DEPTH_BUFFER_BIT);
        // glm::mat4 lightProjection = glm::perspective(glm::radians(this->mainCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        // DirLight *light = (*m_lightContainer->getDirLight())[i];
        // glm::mat4 lightView = glm::lookAt(light->position, light->position + light->direction, glm::vec3(0.f, 1.f, 0.f));
        // glm::mat4 lightTransform = lightProjection * lightView;

        // this->draw(lightTransform, bird, items, trees, lightSources, terrain);
    }


    for(int i=0; i < m_numDepthMaps; i++){
        // glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFrames[i]);
        // glClear(GL_DEPTH_BUFFER_BIT);
        // ConfigureShaderAndMatrices();
        // RenderScene();
    }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shadow::~Shadow()
{
}
