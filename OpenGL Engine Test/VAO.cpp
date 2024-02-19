#include "VAO.h"

//Vertex array constructor
VAO::VAO() {
	glGenVertexArrays(1, &ID);;
}

//Links vertex buffer to vertex array attribute
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset) {
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

//Binds vertex array
void VAO::Bind() {
	glBindVertexArray(ID);
}

//Unbinds vertex array
void VAO::Unbind() {
	glBindVertexArray(0);
}

//Deletes vertex array
void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}