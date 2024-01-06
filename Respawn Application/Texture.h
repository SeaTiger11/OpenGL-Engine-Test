#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shaderClass.h"

class Texture {
	public:
		//Reference ID
		GLuint ID;
		GLenum type;
		//Constructor
		Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

		//Sets the texture variable ID in the fragment shader
		void TexUnit(Shader& shader, const char* uniform, GLuint unit);
		//Binds texture
		void Bind();
		//Unbinds texture
		void Unbind();
		//Deletes texture
		void Delete();
};

#endif