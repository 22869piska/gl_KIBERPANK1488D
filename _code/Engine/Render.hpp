#pragma once

#include "67.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>

class Engine;

class Render
{
 public:
     //data ->
     Engine* p = nullptr;
     ShaderIDs shader_id;
     Vertices vertex;
     
     

     //proc ->
    Render();
    ~Render();

    void GPU();

 private:
    void SpaceView();
    void InitShaders();
    

};