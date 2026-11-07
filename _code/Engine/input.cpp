#include "engine.hpp"

void InputState::InputClear()
{
    static const auto& NATIVEPTR = Engine::native_ptr;
    GLFWwindow* win = NATIVEPTR->window.window;
    //mouse->
    mouse.left_clicked_this_frame = false;
    mouse.right_clicked_this_frame = false;
    mouse.scroll_y = 0;
    glm::vec2 curpos = NATIVEPTR->gui.ScreenToWorldOrtho(); mouse.cursor_pos_in_worldX = curpos.x;mouse.cursor_pos_in_worldY = curpos.y;
   
    //keyboard->
    auto check = [win](int key) -> bool {  return glfwGetKey(win, key) == GLFW_PRESS;};

    // --- A L P H A B E T (A-Z) --->
    for (int i = 0; i < 26; ++i) {
        keys_alpha[i] = check(GLFW_KEY_A + i);
    }

    // --- N U M B E R S (0-9) --->
    for (int i = 0; i < 10; ++i) {
        keys_num[i] = check(GLFW_KEY_0 + i);
    }

    // --- A R R O W S --->
    UP = check(GLFW_KEY_UP);
    DOWN = check(GLFW_KEY_DOWN);
    LEFT = check(GLFW_KEY_LEFT);
    RIGHT = check(GLFW_KEY_RIGHT);

    // --- S P E C I A L --->
    ESCAPE = check(GLFW_KEY_ESCAPE);
    ENTER = check(GLFW_KEY_ENTER);
    BACKSPACE = check(GLFW_KEY_BACKSPACE);
    SPACE = check(GLFW_KEY_SPACE);
    TAB = check(GLFW_KEY_TAB);
    CAPS_LOCK = check(GLFW_KEY_CAPS_LOCK);
    EQUAL = check(GLFW_KEY_EQUAL);
    MINUS = check(GLFW_KEY_MINUS);

    // --- M O D I F I E R S --->
    LSHIFT = check(GLFW_KEY_LEFT_SHIFT);
    RSHIFT = check(GLFW_KEY_RIGHT_SHIFT);
    LCTRL = check(GLFW_KEY_LEFT_CONTROL);
    RCTRL = check(GLFW_KEY_RIGHT_CONTROL);
    LALT = check(GLFW_KEY_LEFT_ALT);
    RALT = check(GLFW_KEY_RIGHT_ALT);
   
}