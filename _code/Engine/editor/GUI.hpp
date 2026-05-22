#pragma once

#define MAX_GUI_VERTICES 4096

struct GuiVertex {
    float x[MAX_GUI_VERTICES];
    float y[MAX_GUI_VERTICES];
    unsigned int color[MAX_GUI_VERTICES];
};

struct Gui{
    
    void MenuManager();

    void MainMenuGui();
    void EditorMenuGui();
};