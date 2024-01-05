#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.H"
#include "EBO.h"

static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

int main() {
	//GLFW error configuration and initialization
	glfwSetErrorCallback(&glfwError);
	glfwInit();

	//Tell glfw what version is being used and what profile to run
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Default array of vertices
	GLfloat vertices[] = {
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, //Left corner
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f, //Right corner
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f, //Top corner
		- 0.25f, 0.5f * float(sqrt(3)) / 6, 0.0f, //Inner left
		0.25f, 0.5f * float(sqrt(3)) / 6, 0.0f, //Inner right
		0.0f, -0.5f * float(sqrt(3)) / 3, 0.0f //Inner bottom
	};

	GLuint indices[] = {
		0, 3, 5, //Bottom Left Triangle
		3, 2, 4, //Bottom Right Triangle
		5, 4, 1 //Top Triangle
	};

	//Creating the window and then checking the process didn't fail
	GLFWwindow* window = glfwCreateWindow(800, 800, "Demo Game", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Load glad to configure openGL
	gladLoadGL();

	//Set openGL viewport
	glViewport(0, 0, 800, 800);

	//Creates shader object with default.vert and default.frag shaders
	Shader shaderProgram("default.vert", "default.frag");

	//Generates vertex array object and binds it
	VAO VAO1;
	VAO1.Bind();

	//Generates vertex and edge buffers and links them to vertices and indices
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	//Links vertex buffer to vertex array
	VAO1.LinkVBO(VBO1, 0);
	//Unbinds all objects to prevent accidental modifications
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//Main game loop
	while (!glfwWindowShouldClose(window)) 
	{
		//Configure background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Set background colour to back buffer
		glClear(GL_COLOR_BUFFER_BIT);
		//Tell openGL what shaders to use
		shaderProgram.Activate();
		//Bind the vertex array so openGL knows to use it
		VAO1.Bind();
		//Draw all the triangles
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		//Swap the back and front buffers
		glfwSwapBuffers(window);

		//Handles all glfw events
		glfwPollEvents();
	}

	//Delete all created objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	//Destroy window then terminate glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}