#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.H"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"

const unsigned int width = 800, height = 800;

//GLFW error logger
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

	//Array of vertices 
	//3 numbers for the position, 3 numbers for the rgb colours, 2 numbers for the texture cords)
	GLfloat vertices[] = { //Currently configured as a pyramid
		-0.5f, 0.0f, 0.5f,		1, 0, 0,	 0.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		1, 0, 0,	 1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1, 0, 0,	 0.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1, 0, 0,	 1.0f, 0.0f,
		0.0f, 0.8f, 0.0f,		1, 0, 0,	 0.5f, 1.0f,
	};

	GLuint indices[] = {
		0, 1, 2,
		0, 2, 3,
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
	};

	//Creating the window and then checking the process didn't fail
	GLFWwindow* window = glfwCreateWindow(width, height, "Demo Game", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Load glad to configure openGL
	gladLoadGL();

	//Set openGL viewport
	glViewport(0, 0, width, height);

	//Creates shader object with default.vert and default.frag shaders
	Shader shaderProgram("default.vert", "default.frag");

	//Generates vertex array object and binds it
	VAO VAO1;
	VAO1.Bind();

	//Generates vertex and edge buffers and links them to vertices and indices
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	//Links vertex buffer to vertex array attributes
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//Unbinds all objects to prevent accidental modifications
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	//Generates texture
	Texture rainbow("rainbow.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	rainbow.TexUnit(shaderProgram, "tex", 0);

	//Enables depth buffer
	glEnable(GL_DEPTH_TEST);

	//Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.5f, 2.0f));

	std::chrono::steady_clock::time_point lastUpdate;
	//Main game loop
	while (!glfwWindowShouldClose(window))
	{
		//Configure background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Clear the colour and depth buffer of the back buffer, then set the new background colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Tell openGL what shaders to use
		shaderProgram.Activate();

		//Gets delta time to ensure that inputs aren't frame rate dependant
		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
		lastUpdate = now;

		//Updates inputs for the camera
		camera.Inputs(window, deltaTime);
		//Sets camera position and orientation
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		//Binds the rainbow texture
		rainbow.Bind();
		//Bind the vertex array so openGL knows to use it
		VAO1.Bind();
		//Draw all the triangles
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//Swap the back and front buffers
		glfwSwapBuffers(window);

		//Handles all glfw events
		glfwPollEvents();
	}

	//Delete all created objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	rainbow.Delete();
	shaderProgram.Delete();

	//Destroy window then terminate glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}