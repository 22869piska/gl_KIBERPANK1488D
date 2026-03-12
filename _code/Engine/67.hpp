#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include"SHADER.hpp"

#define MAX_BLOCK  1000
#define MAX_MODIFICATORS  1000


struct Vertices
{
public:

    float triangle[12] = {
        // positions          // colors
         0.5f,  0.5f, 0.0f,    // top right
         0.5f, -0.5f, 0.0f,    // bottom right
        -0.5f, -0.5f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,
    };
    unsigned int indices[6] = {
       0, 1, 3, // first triangle
       1, 2, 3  // second triangle
    };
    //square_tex->
    float square_tex[20] = {
        // positionsy// texture coords
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f
    };
};
//структура для данных контекста
struct Window
{
    GLFWwindow* window;
    int width = 1280;
    int height = 840;
};
struct ShaderIDs {

    Shader* based_shader;   // 8 байт
    Shader* texture_shader; // 8 байт
    GLuint VAO_triangle;    // 4 байт
    GLuint VBO_triangle;    // 4 байт
    GLuint EBO;             // 4 байт
    GLuint texture_VAO;     // 4 байт
    GLuint texture_VBO;     // 4 байт
    GLuint texture_EBO;     // 4 байт
}; 
struct alignas(64) MVP {
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 mvp;
};
struct SpawnInfo{
    glm::vec2 pos;
    glm::vec2 scale;
    glm::vec2 rotate;
    glm::vec3 color;
    unsigned int tex;
};
//game//
struct alignas(64) Block { 
    alignas(32) glm::vec2 pos[MAX_BLOCK];          // 8 байт 
    alignas(32) glm::vec2 scale[MAX_BLOCK];          // 8 байт 
    alignas(32) glm::vec1 rotate[MAX_BLOCK];       // 4 байт
    alignas(32) glm::vec3 color[MAX_BLOCK];        // 12 байт
    alignas(32) unsigned int texture[MAX_BLOCK];   // 4 байта
    unsigned int count = 0;
    

    //"методы"
    void _fastcall add(const SpawnInfo& info)
    {
        if (count >= MAX_BLOCK) { std::cout << "BLOCK_LIMIT \n"; return; }
       
            pos[count] = info.pos;
            scale[count] = info.scale;
         rotate[count] = info.rotate;
          color[count] = info.color;
        texture[count] = info.tex;

        count++;
    }
    void _fastcall remove(unsigned int index)
    {
        if (index >= count) { std::cout << "INDEX > CURRENT_COUNT \n";  return; };
        count--;

        if (index != count) {
            pos[index] = pos[count];
            scale[index] = scale[count];
            rotate[index] = rotate[count];
            color[index] = color[count];
            texture[index] = texture[count];
        }
    }

};






















//END