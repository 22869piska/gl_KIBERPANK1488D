#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
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

#include "../asm/asm.hpp"
#include "67.hpp"
#include "obj.hpp"
#include "Render.hpp"
#include "Textures.hpp"
#include "editor/editor.hpp"
#include "input.hpp"


class Engine
{
    friend class Render;
    friend struct IO;
public:
 //data ->
   
    

 //proc ->
    Engine();
    ~Engine();

    void init();
    void Run();
    
  
private:
    bool isRunning = false;
    // Резервируем место под объект БЕЗ вызова конструктора
    alignas(Render) char render_mem[sizeof(Render)];
    Render* render = nullptr;
    alignas(64) char block_mem[sizeof(Block)];
    Block* block = nullptr;
    alignas(64) char light_mem[sizeof(Light)];
    Light* light = nullptr;
    // для кеша
    alignas(64) float batch_block_ptr[MAX_BLOCK * 20];
    alignas(64) float batch_light_ptr[MAX_LIGHTS * 32];
    //еще резервация под код  
    //                                                                            // code_size   //
    unsigned long long code_size = 64 * 1024;void* runtime_code = VirtualAlloc(NULL, code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    unsigned char* code_ptr = (unsigned char*)this->runtime_code;

    Textures texture;
    Window window;
    MVP mvp;
    SpawnInfo param;
    //proc ->
    void macro_def();
    void initGlContext();
    //
    void CPU();
    void Tps();
    void SaveLevel();
    
    //
    void SpaceView();
    void Batch();
    void Block_batch();
    void Light_batch();
    //

};
