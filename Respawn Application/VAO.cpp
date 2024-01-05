#include "VAO.h"

//Vertex array constructor
VAO::VAO() {
	glGenVertexArrays(1, &ID);;
}

//Links vertex buffer object to vertex array object
void VAO::LinkVBO(VBO VBO, GLuint layout) {
	VBO.Bind();
	glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

//Binds vertex array object
void VAO::Bind() {
	glBindVertexArray(ID);
}

//Unbinds vertex array object
void VAO::Unbind() {
	glBindVertexArray(0);
}

//Deletes vertex array object
void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}