#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO {
	public:
		//Reference ID
		GLuint ID;
		//Constructor
		EBO(GLuint* indices, GLsizeiptr size);

		//Binds edge buffer object
		void Bind();
		//Unbinds edge buffer object
		void Unbind();
		//Deletes edge buffer object
		void Delete();
};

#endif