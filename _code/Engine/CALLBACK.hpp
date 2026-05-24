#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

namespace Callback{
    
    void FramebufferSize(GLFWwindow* window, int width, int height);
    void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);


   // void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}