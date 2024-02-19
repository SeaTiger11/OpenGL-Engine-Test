#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>
#include <vector>

class EBO {
	public:
		//Reference ID
		GLuint ID;
		//Constructor
		EBO(std::vector<GLuint>& indices);

		//Binds edge buffer
		void Bind();
		//Unbinds edge buffer
		void Unbind();
		//Deletes edge buffer
		void Delete();
};

#endif