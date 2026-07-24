#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>

class Textures
{
public:
	static constexpr unsigned int MAX_TEXTURES = 2;
	const char* paths[MAX_TEXTURES] = {
	"_code/TEXTURES/ventilator.png", // 1
	"_code/TEXTURES/dildo.png",      // 2
	};
	enum Layers {
		VENTILATOR = 0,
		VENTILATOR_2 = 1
	};
	unsigned int texFont;
	unsigned int texArray;

	unsigned int LoadTextureArray();
	unsigned int LoadTexture2d(const char* filename);

	//;---------------------------------------------------------------------------------------------------------

	
};