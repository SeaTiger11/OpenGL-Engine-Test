#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

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

	//Array of vertices 
	//3 numbers for the position, 3 numbers for the rgb colours, 2 numbers for the texture cords)
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,	 1, 0, 0,	 0, 0, //Bottom left
		-0.5f, 0.5f, 0.0f,	 0, 1, 0,	 0, 1, //Top left
		0.5f, 0.5f, 0.0f,	 0, 0, 1,	 1, 1, //Bottom right
		0.5f, -0.5f, 0.0f,	 0, 0, 0,	 1, 0 //Top right
	};

	GLuint indices[] = {
		0, 2, 1, //Upper triangle
		0, 3, 2 //Lower triangle
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

	//Variables for image properties to be stored
	int imgWidth, imgHeight, numColourChannels;
	//By default stb loads image differently to openGL resulting in flipped image
	stbi_set_flip_vertically_on_load(true);
	//Turns image into byte array and updates image property variables
	unsigned char* imgBytes = stbi_load("titanfall2.png", &imgWidth, &imgHeight, &numColourChannels, 0); //Get image bytes

	//Generates a 2D texture
	GLuint texture;
	glGenTextures(1, &texture);
	//Assigns texture to texture unit
	glActiveTexture(GL_TEXTURE0);
	//Binds texture
	glBindTexture(GL_TEXTURE_2D, texture);

	//Interpolates new pixels when scaling (may make image a bit blurry)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Makes image not repeat and instead simply have a border colour
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//Generates a colour then sets it as the border colour for the texture
	float flatColour[] = { 1, 1, 1, 1 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColour);

	//Sets the texture data to the previously created image bytes
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBytes);
	//Generates image mipmaps (smaller versions of the same image)
	glGenerateMipmap(GL_TEXTURE_2D);

	//Delete the image bytes
	stbi_image_free(imgBytes);
	//Unbind the texture to prevent accidental modifications
	glBindTexture(GL_TEXTURE_2D, 0);

	//Gets the texture variable ID in the vertex shader
	GLuint tex0Uni = glGetUniformLocation(shaderProgram.ID, "tex0");
	shaderProgram.Activate();
	glUniform1i(tex0Uni, 0);

	//Main game loop
	while (!glfwWindowShouldClose(window)) 
	{
		//Configure background colour
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		//Set background colour to back buffer
		glClear(GL_COLOR_BUFFER_BIT);
		//Tell openGL what shaders to use
		shaderProgram.Activate();
		//Sets the scale of the vertex shader
		glUniform1f(uniID, 1.5f);
		glBindTexture(GL_TEXTURE_2D, texture);
		//Bind the vertex array so openGL knows to use it
		VAO1.Bind();
		//Draw all the triangles
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//Swap the back and front buffers
		glfwSwapBuffers(window);

		//Handles all glfw events
		glfwPollEvents();
	}

	//Delete all created objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	glDeleteTextures(1, &texture);
	shaderProgram.Delete();

	//Destroy window then terminate glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}