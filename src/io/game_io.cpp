#include "game_io.h"
#include <src/commons/common_rendering.h>
#include <utils/glPrerequisites.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera *camera_for_callback;
Entity *entity_for_callback;
bool capture_cursor = false;

GameIO::GameIO(GLFWwindow* w, Camera *c) : window(w), camera(c) {}

void GameIO::init(){
    // capture cursor mouse
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(this->window, mouse_callback);
    glfwSetScrollCallback(this->window, scroll_callback);
    camera_for_callback = camera;
}

void GameIO::bindEntity(Entity * e){
    entity = e;
    entity_for_callback = entity;
}

void GameIO::poll(float deltaTime){
    if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->window, true);
    glfwPollEvents();
    this->pollCameraIO(deltaTime);
    this->pollEntityIO(deltaTime);
}

// not a callback function
void GameIO::pollCameraIO(float deltaTime){
	// if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
	// 	camera->ProcessKeyboard(FORWARD, deltaTime);
	// if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
	// 	camera->ProcessKeyboard(BACKWARD, deltaTime);
	// if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
	// 	camera->ProcessKeyboard(LEFT, deltaTime);
	// if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
	// 	camera->ProcessKeyboard(RIGHT, deltaTime);
	// if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	// 	camera->Position += glm::normalize(glm::cross(camera->Right, camera->Front)) * camera->MovementSpeed * deltaTime;
    // if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	// 	camera->Position -= glm::normalize(glm::cross(camera->Right, camera->Front)) * camera->MovementSpeed * deltaTime;
    if (glfwGetKey(this->window, GLFW_KEY_C) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        capture_cursor = false;  
    }
    else{
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        capture_cursor = true;
       
    }

}

void GameIO::pollEntityIO(float deltaTime){
    // static bool pressing = false;
    // if(glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS){
    //     if(!pressing){
    //         this->entity->setVelocity(glm::vec3(0,this->entity->jumpVel,0));
    //         // std::cerr << "JUMP"<<std::endl;
    //         pressing = true;
    //     }
    // }
    // else{
    //     pressing = false;
    // }
    
    static bool isPressing = false;
    static double pressStartTime = 0.f;
    if(glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS && !isPressing){
        pressStartTime = glfwGetTime();
        isPressing = true;
    }
    else if(glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_RELEASE && isPressing){
        double pressEndTime = glfwGetTime() - pressStartTime;
        this->entity->setVelocity(glm::vec3(0,this->entity->jumpVel * pressEndTime * 3.0 ,0));
        isPressing = false;
    }



    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS){
		this->entity->move(glm::vec3(0.05,0,0));
        // this->entity->setAcceleration(this->entity->getAcceleration() + glm::dot(glm::vec3(0.01,0,0), this->entity->getFront()) * this->entity->getFront());
        }
	// if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS){
		// this->entity->move(glm::vec3(-0.1,0,0));
        // this->entity->setAcceleration(this->entity->getAcceleration() + glm::dot(glm::vec3(-0.01,0,0), this->entity->getFront()) * this->entity->getFront());
        // }
    
    static float rotAngle = 0.f;
    
    // rotaxis.y = 1;
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS){
        rotAngle += 0.003f;
        // this->entity->setOrientation(front-glm::vec3(-0.1,0,0))
        }
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS){
        rotAngle -= 0.003f;
        // this->entity->rotatePhysics(rotaxis,-0.1f);
		// this->entity->move(glm::vec3(0,0,0.1));
        }
    glm::vec3 heading = glm::vec3(glm::sin(rotAngle), 0, glm::cos(rotAngle));
    this->entity->setOrientation(heading, 0);
    this->camera->Position = entity->getPosition() - 10.f * heading + glm::vec3(0,4.f,0);
    this->camera->Front = glm::normalize(heading - glm::vec3(0,0.15,0));
    // this->camera->Front
	if (glfwGetKey(this->window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS){
        // this->entity->setVelocity(this->entity->getVelocity() + glm::vec3(0,0.1,0));
        this->entity->setPosition(this->entity->getPosition() + glm::vec3(0,0.1,0));
    }
    if (glfwGetKey(this->window, GLFW_KEY_SLASH) == GLFW_PRESS){
        // this->entity->setVelocity(this->entity->getVelocity() - glm::vec3(0,0.1,0));
        this->entity->setPosition(this->entity->getPosition() - glm::vec3(0,0.1,0));
    }

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
    glViewport(0,0,SCR_WIDTH,SCR_HEIGHT);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    static double lastX = SCR_WIDTH / 2;
    static double lastY = SCR_HEIGHT / 2;

    double xOff = xpos - lastX;
    double yOff = ypos - lastY;
    
    lastX = xpos;
    lastY = ypos;

    // if(capture_cursor)
    //     camera_for_callback->ProcessMouseMovement(xOff, -yOff);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera_for_callback->ProcessMouseScroll(yoffset);
}