#include "obj.hpp"


  void _fastcall Block::add(const SpawnInfo& info)
  {
      if (count >= MAX_BLOCK) { std::cout << "BLOCK_LIMIT \n"; return; }

      pos_X[count] = info.pos.x;
      pos_Y[count] = info.pos.y;
      scale_X[count] = info.scale.x;
      scale_Y[count] = info.scale.y;
      rotate_A[count] = info.rotate.x; 
      color_R[count] = info.color.r;
      color_G[count] = info.color.g;
      color_B[count] = info.color.b;
      texture[count] = info.tex;

      count++;
  }
  void _fastcall Block::remove(unsigned int index)
  {
      if (index >= count) return;
      count--;

      if (index != count) {
          pos_X[index] = pos_X[count];
          pos_Y[index] = pos_Y[count];
          scale_X[index] = scale_X[count];
          scale_Y[index] = scale_Y[count];
          rotate_A[index] = rotate_A[count];
          color_R[index] = color_R[count];
          color_G[index] = color_G[count];
          color_B[index] = color_B[count];
          texture[index] = texture[count];
      }
  }
//
  void _fastcall Light::add(float x, float y, float r, float g, float b, float rad, float intens)
  {
      if (count >= MAX_LIGHTS) return;
      pos_X[count] = x; pos_Y[count] = y;
      color_R[count] = r; color_G[count] = g; color_B[count] = b;
      radius[count] = rad; intensity[count] = intens;
      count++;
  }