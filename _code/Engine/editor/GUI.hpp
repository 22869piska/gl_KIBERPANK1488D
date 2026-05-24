#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#define MAX_GUI_VERTICES 4096

struct GuiVertex {
    float x[MAX_GUI_VERTICES];
    float y[MAX_GUI_VERTICES];
    float colorR[MAX_GUI_VERTICES];
    float colorG[MAX_GUI_VERTICES];
    float colorB[MAX_GUI_VERTICES];
    unsigned int count = 0;
};

struct Gui{
    
    

    void MenuManager();

  
    
    void MainMenuGui();
    void EditorMenuGui();
    //
    bool ADDButton(float x,float y, const glm::vec3& color);
   
};