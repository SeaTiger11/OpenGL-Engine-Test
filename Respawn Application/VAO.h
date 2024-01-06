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

		//Links vertex buffer to vertex array attribute
		void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);
		//Binds vertex array
		void Bind();
		//Unbinds vertex array
		void Unbind();
		//Deletes vertex array
		void Delete();
};

#endif