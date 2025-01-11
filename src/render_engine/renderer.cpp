#include "renderer.h"
#include <utils/glPrerequisites.h>
#include <utils/learnopenglPrerequisites.h>
#include <src/lighting/lighting.h>
#include <src/commons/common_rendering.h>
#include <src/commons/common_global.h>
// #include <src/commons/common_entities.h>
#include <utils/include/axis.h>
#include <utils/include/sphere.h>
#include <utils/include/iniparser.hpp>

#include <iostream>

int SCR_WIDTH;
int SCR_HEIGHT;

float cursorLastX = SCR_WIDTH / 2;
float cursorLastY = SCR_HEIGHT / 2;
bool firstMouse = true;

Renderer::Renderer(){
}

Renderer::~Renderer(){
    // delete dynamicShader;
}

// set shader and GLs
void Renderer::init(string settingPath){
    INI::File ft;
	if (!ft.Load(settingPath))
	{
		// Loading from stream
		std::ifstream op("opts.ini",std::ios::in);
		if (op.is_open())
			op >> ft;
	}

    // shader init
    // CHANGE NUM_LIGHT AT FRAGEMENT SHADER
    string SHADER_DIR = ft.GetValue("SHADER_DIR").AsString();
    string SHADER_DYNAMIC_VERT = SHADER_DIR + ft.GetValue("SHADER_DYNAMIC_VERT").AsString();
    string SHADER_DYNAMIC_FRAG = SHADER_DIR + ft.GetValue("SHADER_DYNAMIC_FRAG").AsString();
    this->dynamicShader = new Shader(SHADER_DYNAMIC_VERT.c_str(), SHADER_DYNAMIC_FRAG.c_str());

    string SHADER_STATIC_VERT = SHADER_DIR + ft.GetValue("SHADER_STATIC_VERT").AsString();
    string SHADER_STATIC_FRAG = SHADER_DIR + ft.GetValue("SHADER_STATIC_FRAG").AsString();
    this->staticShader = new Shader(SHADER_STATIC_VERT.c_str(), SHADER_STATIC_FRAG.c_str());

    string SHADER_TREE_VERT = SHADER_DIR + ft.GetValue("SHADER_TREE_VERT").AsString();
    this->instancingShader = new Shader(SHADER_TREE_VERT.c_str(), SHADER_DYNAMIC_FRAG.c_str());

    string SHADER_TERRAIN_VERT = SHADER_DIR + ft.GetValue("SHADER_TERRAIN_VERT").AsString();
    string SHADER_TERRAIN_FRAG = SHADER_DIR + ft.GetValue("SHADER_TERRAIN_FRAG").AsString();

    this->groundShader = new Shader(SHADER_TERRAIN_VERT.c_str(), SHADER_TERRAIN_FRAG.c_str());

    // input init
    cursorLastX = SCR_WIDTH / 2.f;
    cursorLastY = SCR_HEIGHT / 2.f;    
    firstMouse = true;

    // camera init
    glm::vec3 pos, up;
    float yaw, pitch;
    for(int i=0;i<3;i++){
        pos[i] = ft.GetValue("MAINCAMERA_POS").AsArray()[i].AsDouble();
        up[i] = ft.GetValue("MAINCAMERA_UP").AsArray()[i].AsDouble();
    }
    yaw = glm::radians(ft.GetValue("MANICAMERA_YAW").AsDouble());
    pitch = glm::radians(ft.GetValue("MAINCAMERA_PITCH").AsDouble());

    // this->mainCamera = new Camera(pos, up, yaw, pitch);
    this->mainCamera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));

    // glEnable(GL_DEPTH_TEST);

}


void Renderer::draw(LightContainer &lightContainer, EntityDynamic* bird, std::vector<EntityStaticInstanced*> items, std::vector<EntityStaticInstanced*> trees, std::vector<EntityStatic*> lightSources, TerrainDiaSq* terrain){

    // this->mainCamera.Position = bird->getPosition() + glm::vec3(-3,1,0);
    // this->mainCamera.Front = glm::vec3(1, -0.2, 0);
    // bird->setOrientation(this->mainCamera.Front, 0);
    // bird->setPosition(this->mainCamera.Position + glm::vec3(3, -1, 0));
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.529f,0.808f,0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // CAMERA STICK
    // this->mainCamera.Position = bird->getPosition() - 10.f * bird->getFront() + glm::vec3(0,2.f,0);
    // this->mainCamera.Front = bird->getPosition() - this->mainCamera.Position;
    // DYNAMICS
    // update light
    this->dynamicShader->use();
    lightContainer.setToShader(dynamicShader);
    // lightContainer.printNum();

    // view/projection transformations
    glm::mat4 view = this->mainCamera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(this->mainCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
    this->dynamicShader->setMat4("projection", projection);
    // this->dynamicShader->setMat4("projection", glm::mat4(1.0f));
    this->dynamicShader->setMat4("view", view);
    // this->dynamicShader->setMat4("view", glm::mat4(1.0f));
    
    // draw
    if(bird != NULL) {
        // bird->updateAnimation(deltaTime);
        bird->draw(this->dynamicShader);
    }

    // STATICS : INSTANCING
    this->instancingShader->use();
    lightContainer.setToShader(instancingShader);

    this->instancingShader->setMat4("projection", projection);
    this->instancingShader->setMat4("view", view);

    for(auto i : items){    
        i->draw(this->instancingShader);
    }
    for(auto t : trees){
        t->draw(this->instancingShader);
    }

    // GROUND
    this->groundShader->use();
    lightContainer.setToShader(groundShader);

    this->groundShader->setMat4("projection", projection);
    this->groundShader->setMat4("view", view);
    
    if(terrain != nullptr) { terrain->Draw(this->groundShader); }

    // STATICS : NO INSTANCING
    this->staticShader->use();
    // lightContainer.setToShader(staticShader);

    this->staticShader->setMat4("projection", projection);
    this->staticShader->setMat4("view", view);
    // glDepthMask(GL_FALSE);
    for(auto l : lightSources){
        l->draw(this->staticShader);
    }
    // glDepthMask(GL_TRUE);


    // some debuggins
    // static Axis axis;
    // static glm::mat4 axisModel(1.f);
    // axis.draw(axisModel, view,projection);

    // static Sphere sphere(1.f);
    // // offsetmat has already scaled
    // glm::mat4 birdSphere = glm::translate(glm::mat4(1.f), bird->getPosition());
    // sphere.draw(birdSphere, view, projection);


    // if(shadow == nullptr){
    //     shadow = new Shadow(&lightContainer, "./settings.ini");
    // }
    // shadow->renderDepthMap(bird,items,trees,lightSources,terrain);
}

Camera* Renderer::getCamera()
{
    return &(this->mainCamera);
}

void Renderer::setWindow(GLFWwindow* window)
{
    this->mainWindow = window;
}

