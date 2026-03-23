#pragma once
#define _CRT_SECURE_NO_WARNINGS
//#include <Windows.h>

#include <cstdio>
#include <cstdint>
#include <iostream>


namespace BIN_IO {
	bool Save(const void* data, size_t size,const char name[]);
	bool SaveAppend(const void* data, size_t size, const char name[]);
	bool Load(void* data, size_t size, const char name[], unsigned int OFFSET);
};