#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO {
	public:
		//Reference ID
		GLuint ID;
		//Constructor
		VBO(GLfloat* vertices, GLsizeiptr size);

		//Binds vertex buffer
		void Bind();
		//Unbinds vertex buffer
		void Unbind();
		//Deletes vertex buffer
		void Delete();
};

#endif