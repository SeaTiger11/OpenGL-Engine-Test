#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.H"
#include "EBO.h"
#include "Texture.h"

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

	//Gets the scale variable ID in the vertex shader
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	//Generates texture
	Texture rainbow("rainbow.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	rainbow.TexUnit(shaderProgram, "tex", 0);

	//Variables to assist with rotating the pyramid
	float rotation = 0.0f;
	double prevTime = glfwGetTime();

	//Enables depth buffer
	glEnable(GL_DEPTH_TEST);

	//Main game loop
	while (!glfwWindowShouldClose(window))
	{
		//Configure background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Clear the colour and depth buffer of the back buffer, then set the new background colour
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Tell openGL what shaders to use
		shaderProgram.Activate();

		//Simple timer to assist with rotation
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60) {
			rotation += 0.1f;
			prevTime = crntTime;
		}

		//Initialize matrices to prevent them being NULL 
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		//Rotate model over time
		model = glm::rotate(model, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		//Offset the view position
		view = glm::translate(view, glm::vec3(0, -0.5f, -5.0f));
		//Set the view frustum
		proj = glm::perspective(glm::radians(45.0f), float(width / height), 0.1f, 100.0f);

		//Updates the vertex shader uniforms to include the matrices required for 3D
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));


		//Sets the scale of the vertex shader
		glUniform1f(uniID, 1.5f);
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