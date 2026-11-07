#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
#include <bitset>
#include <filesystem>
#include <cmath>
#include <thread>   
#include <chrono> 
#include <new>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")


#include "67.hpp"
#include "obj.hpp"
#include "Render.hpp"
#include "Textures.hpp"
#include "editor/editor.hpp"
#include "input.hpp"
#include "editor/GUI.hpp"

#include "jit.hpp"

#define MY_MEM_SIZE 128

class Engine
{
    friend class Render;
    friend struct IO;
    friend struct Gui;
    friend struct EDITOR;
    friend struct InputState;
public:
 
    //game obj внутри bin ->
    char basealign[256];
    
    alignas(64) char block_mem[sizeof(Block)];
    Block* block = nullptr;
    alignas(64) char light_mem[sizeof(Light)];
    Light* light = nullptr;

    //Код
    alignas(16) unsigned char MY_MEM[MY_MEM_SIZE]; //  128 байт "памяти ассемблера"

    //game obj не копируемые (озу only)->
    //Cam
    float camera_posX = 0;float camera_posY = 0;
    float camera_zoom = 1;
    //inpt
    InputState input_state; 

    // sys ->
    // для кеша
    alignas(64) float batch_block_ptr[MAX_BLOCK * 20];
    alignas(64) float batch_light_ptr[MAX_LIGHTS * 32];

    //менять оффсет памяти только после всех обьектов , а не до их создания(памяти)
    // резерв для gui !!! не будет использоваться как память для bin и игры в целом это лишь gui !!!
    alignas(64) char gui_vertex_mem[sizeof(GuiVertex)];
    GuiVertex* gui_vertex = nullptr;
      // Резервируем место под объект БЕЗ вызова конструктора
    alignas(64) char render_mem[sizeof(Render)];
    Render* render = nullptr;

    //data ->
    static Engine* native_ptr;
    bool isGameMode = false;
    bool isMainMenu = true;
    bool isEditorMode = false;


    Textures texture;
    Window window;
    MVP mvp;
    //SpawnInfo param;
    Gui gui;
    EDITOR editor;
    //sys

    //proc ->
    Engine();
    ~Engine();

    void init();
    void Run();
private:

    void macro_def();
    void initGlContext();
    void InitCallBack();
    
    //
    void CPU();
    void Tps();
    void SaveLevel();
    bool SaveCodeInLevelFileFromFile();
    void LoadCodeInMemoryFromFile();
    bool LoadLevel();
    void GetAllAddrObjOffset();
    
    //
    void SpaceView();
    void Batch();
    void Block_batch();
    void Light_batch();
    //

};
