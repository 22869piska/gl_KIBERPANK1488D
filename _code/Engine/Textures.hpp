#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>



#include <iostream>

class Textures
{
public:
	static constexpr unsigned int MAX_TEXTURES = 2;
	const char* paths[MAX_TEXTURES] = {
	"_code/TEXTURES/ventilator.png",
	"_code/TEXTURES/dildo.png",
	};
	enum Layers {
		VENTILATOR = 0,
		VENTILATOR_2 = 1
	};
	unsigned int texArray;
	unsigned int LoadTextureArray();

	//;---------------------------------------------------------------------------------------------------------

	
};