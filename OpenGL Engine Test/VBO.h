#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

//Struct to represent a vertex
struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 colour;
	glm::vec2 texUV;
};

class VBO {
	public:
		//Reference ID
		GLuint ID;
		//Constructor
		VBO(std::vector<Vertex>& vertices);

		//Binds vertex buffer
		void Bind();
		//Unbinds vertex buffer
		void Unbind();
		//Deletes vertex buffer
		void Delete();
};

#endif