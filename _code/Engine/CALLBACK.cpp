#include "CALLBACK.hpp"
#include <iostream>

#include "..\Engine\engine.hpp"

void Callback::FramebufferSize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Engine::native_ptr->window.width = width;
	Engine::native_ptr->window.height = height;

    if (width <= 0 || height <= 0) { return; }

    Engine::native_ptr->window.scale_X = VIRTUAL_WIDTH / static_cast<float>(width);
    Engine::native_ptr->window.scale_Y = VIRTUAL_HEIGHT / static_cast<float>(height);

}
void Callback::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Engine::native_ptr->mouse.x = static_cast<float>(xpos);
	Engine::native_ptr->mouse.y = static_cast<float>(ypos);
}
void Callback::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            Engine::native_ptr->mouse.left_clicked_this_frame = true;
            Engine::native_ptr->mouse.left_is_down = true;
        }
        else if (action == GLFW_RELEASE)
        {
            Engine::native_ptr->mouse.left_is_down = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            Engine::native_ptr->mouse.right_clicked_this_frame = true;
            Engine::native_ptr->mouse.right_is_down = true;
        }
        else if (action == GLFW_RELEASE)
        {
            Engine::native_ptr->mouse.right_is_down = false;
        }
    }
}
void Callback::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Engine::native_ptr->mouse.scroll_x = (float)xoffset;
    Engine::native_ptr->mouse.scroll_y = (float)yoffset;
}
