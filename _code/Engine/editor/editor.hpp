#pragma once
#define _CRT_SECURE_NO_WARNINGS
//#include <Windows.h>

#include <cstdio>
#include <cstdint>
#include <iostream>

#include "GUI.hpp"


namespace BIN_IO {
	bool Save(const void* data, size_t size,const char name[]);
	bool SaveAppend(const void* data, size_t size, const char name[]);
	bool Load(void* data, size_t size, const char name[], unsigned int OFFSET);
};
namespace EDITOR {
	struct SpawnInfo {
		glm::vec3 color  = glm::vec3(1.0f,1.0f,1.0f);
		glm::vec2 pos    = glm::vec2(0.0f,0.0f);
		glm::vec2 scale  = glm::vec2(50.0f,50.0f);
		glm::vec2 rotate = glm::vec2(0.0f,0.0f);
		unsigned int tex = 0;
		float rad = 50;
		float intens = 1;

	}; inline SpawnInfo spawninfo;
	
	inline float gridsize = 50.f;
	inline bool show_spawn = true;

	//->
	glm::vec2 SnapToGrid(glm::vec2 worldPos);
	void PreviewMode();
}