

class Textures
{
public:
	unsigned int texture_count = 0;

	unsigned int vjlink_granate = 0;
	unsigned int XAMAC_GUN = 0;

	unsigned int LoadTextureRGB(const char* path)
	{
		texture_count++;
		unsigned int my_texture;
		glGenTextures(1, &my_texture);
		glBindTexture(GL_TEXTURE_2D, my_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		return my_texture;
	}
	unsigned int LoadTextureRGBA(const char* path)
	{
		unsigned int my_texture;
		glGenTextures(1, &my_texture);
		glBindTexture(GL_TEXTURE_2D, my_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		stbi_image_free(data);

		return my_texture;

	}
	//;---------------------------------------------------------------------------------------------------------
	void LoadAllTexture()
	{
		vjlink_granate = LoadTextureRGB("_code/TEXTURES/vjlink_granate.jpg");
		XAMAC_GUN = LoadTextureRGB("_code/TEXTURES/XAMAC_GUN.jpg");
		

	}

};