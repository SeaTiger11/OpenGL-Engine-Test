#include "VAO.h"

//Vertex array constructor
VAO::VAO() {
	glGenVertexArrays(1, &ID);;
}

//Links vertex buffer object to vertex array attribute
void VAO::LinkAttrib(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
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