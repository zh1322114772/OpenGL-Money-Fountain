#pragma once
#include <string>
#include "stb_image.h"
#include <iostream>
#include <stdexcept>
class Texture 
{
private:
	unsigned int textureID;
public:

	void Init(const char* file_path) 
	{
		int w, h, c;
		unsigned char* data = stbi_load(file_path, &w, &h, &c, 0);
		if (data == nullptr) 
		{
			throw std::runtime_error("Unable to load the image");
		}

		//load image to vram
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}

	unsigned int getID() 
	{
		return textureID;
	}

};