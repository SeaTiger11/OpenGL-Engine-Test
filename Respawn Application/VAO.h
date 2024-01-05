#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO {
	public:
		//Reference ID
		GLuint ID;
		//Constructor
		VAO();

		//Links vertex buffer object to vertex array object
		void LinkVBO(VBO VBO, GLuint layout);
		//Binds vertex array object
		void Bind();
		//Unbinds vertex array object
		void Unbind();
		//Deletes vertex array object
		void Delete();
};

#endif