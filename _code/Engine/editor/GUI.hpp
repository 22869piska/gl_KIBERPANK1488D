#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#define MAX_GUI_VERTICES 4096

struct GuiVertex {
    float x[MAX_GUI_VERTICES];
    float y[MAX_GUI_VERTICES];
    float u[MAX_GUI_VERTICES]; //msg tex font
    float v[MAX_GUI_VERTICES]; //msg tex font
    float colorR[MAX_GUI_VERTICES];
    float colorG[MAX_GUI_VERTICES];
    float colorB[MAX_GUI_VERTICES];
   
    unsigned int count = 0;
};

struct Gui{
   
    float w, h;
    bool mouse_over_gui = false;

    void MenuManager();
    //
    void MainMenuGui();
    void EditorMenuGui();
    //
    bool ADDButton(float x,float y, const glm::vec3& color,const char* msg);
    float ADDText(float x, float y, const char* text, glm::vec3 color);
    glm::vec2 ScreenToWorldOrtho(float mx,float my, int windowWidth, int windowHeight, const glm::mat4& orthoMatrix);
    void CheckMouseCollisionOnButton();
   
};