#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shaderClass.h"

class Texture {
	public:
		//Reference ID
		GLuint ID;
		const char* type;
		GLuint unit;

		//Constructor
		Texture(const char* image, const char* texType, GLuint slot);

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