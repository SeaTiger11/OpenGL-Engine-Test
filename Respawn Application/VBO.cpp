#include "VBO.h"

//Constructor
VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

//Binds vertex buffer
void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

//Unbinds vertex buffer
void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Deletes vertex buffer
void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}