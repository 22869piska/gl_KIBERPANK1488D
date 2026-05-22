#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>




namespace Callback{
    
    void FramebufferSize(GLFWwindow* window, int width, int height);
    void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
     


   // void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}