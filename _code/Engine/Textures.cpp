#include "Textures.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int Textures::LoadTextureArray()
{
	unsigned int texArray;
	glGenTextures(1, &texArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);

	const int W = 50, H = 50; // Все картинки должны быть 50x50
	//this->MAX_TEXTURES;

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, W, H, this->MAX_TEXTURES, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	stbi_set_flip_vertically_on_load(true);

	for (int i = 0; i < this->MAX_TEXTURES; i++) {
		int w, h, nrChannels;
		// Загружаем пиксели из файла
		unsigned char* data = stbi_load(paths[i], &w, &h, &nrChannels, 4); // 4 = RGBA

		if (data) {
			// 2. Копируем пиксели в i-й слой массива
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, W, H, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "Failed to load: " << paths[i] << std::endl;
		}
	}

	// Параметры фильтрации
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texArray;
}
//;---------------------------------------------------------------------------------------------------------

