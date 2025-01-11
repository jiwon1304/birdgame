#ifndef __DISPLAY_MANAGER_H__
#define __DISPLAY_MANAGER_H__

#include <utils/glPrerequisites.h>
#include <string>

class DisplayManager{
public:
    DisplayManager();
    void init(std::string settingPath);
    GLFWwindow* window;
};
#endif