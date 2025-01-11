#include "display_manager.h"
#include <utils/include/iniparser.hpp>
#include <src/commons/common_rendering.h>

DisplayManager::DisplayManager(){
}

void DisplayManager::init(std::string settingPath){
    INI::File ft;
	if (!ft.Load("settings.ini"))
	{
		// Loading from stream
		std::ifstream op("opts.ini",std::ios::in);
		if (op.is_open())
			op >> ft;
	}

    SCR_WIDTH = ft.GetValue("SCR_WIDTH", 800).AsInt();
    SCR_HEIGHT = ft.GetValue("SCR_HEIGHT", 600).AsInt();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BirdGame", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    
    // configure global opengl state
    // -----------------------------
    
    // apple supersampling
#ifdef __APPLE__
    SCR_WIDTH *= 2;
    SCR_HEIGHT *= 2;
    glViewport(0,0,SCR_WIDTH, SCR_HEIGHT);
#endif

    glEnable(GL_DEPTH_TEST);
}
