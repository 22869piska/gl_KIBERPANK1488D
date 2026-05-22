#include "CALLBACK.hpp"
#include <iostream>

#include "..\Engine\engine.hpp"

void Callback::FramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Engine::native_ptr->window.width = width;
	Engine::native_ptr->window.height = height;
}
void Callback::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Engine::native_ptr->mouse.x = static_cast<float>(xpos);
	Engine::native_ptr->mouse.y = static_cast<float>(ypos);
}
//исправленая