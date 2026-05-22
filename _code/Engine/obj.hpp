#include "67.hpp"


struct alignas(64) Block {

    alignas(32) float pos_X[MAX_BLOCK];
    alignas(32) float pos_Y[MAX_BLOCK];
    alignas(32) float scale_X[MAX_BLOCK];
    alignas(32) float scale_Y[MAX_BLOCK];
    alignas(32) float rotate_A[MAX_BLOCK];
    // Цвета можно хранить как float (R, G, B) или упаковать в один uint (RGBA)
    alignas(32) float color_R[MAX_BLOCK];
    alignas(32) float color_G[MAX_BLOCK];
    alignas(32) float color_B[MAX_BLOCK];
    alignas(32) unsigned int texture[MAX_BLOCK];

    unsigned int count = 0;
    void _fastcall add(const SpawnInfo& info);
    void _fastcall remove(unsigned int index);
   
};
struct alignas(64) CollisionSystem {
    alignas(32) float pos_X[MAX_BLOCK];
    alignas(32) float pos_Y[MAX_BLOCK];
    alignas(32) float half_W[MAX_BLOCK]; // Заранее посчитано: width * 0.5
    alignas(32) float half_H[MAX_BLOCK]; // Заранее посчитано: height * 0.5

    unsigned int count = 0;
    // Сюда добавим методы проверки коллизий позже
};
struct alignas(64) Light {
    alignas(32) float pos_X[MAX_LIGHTS];
    alignas(32) float pos_Y[MAX_LIGHTS];
    alignas(32) float color_R[MAX_LIGHTS];
    alignas(32) float color_G[MAX_LIGHTS];
    alignas(32) float color_B[MAX_LIGHTS];
    alignas(32) float radius[MAX_LIGHTS];
    alignas(32) float intensity[MAX_LIGHTS]; // Сила свечения

    unsigned int count = 0;

    void _fastcall add(float x, float y, float r, float g, float b, float rad, float intens);
       
    
};
