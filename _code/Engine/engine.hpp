#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>
#include <cmath>
#include <thread>   
#include <chrono> 
#include <new>

#include <windows.h>
#pragma comment(lib, "winmm.lib")

#include "../asm/asm.hpp"
#include "67.hpp"
#include "Render.hpp"
#include "Textures.hpp"


class Engine
{
    friend class Render;
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
    //alignas(64) glm::mat4 model[MAX_BLOCK];
    alignas(64) float batch_ptr[MAX_BLOCK * 20];
    // для кеша
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
    //
    void SpaceView();
    void Batch();
    //

};
