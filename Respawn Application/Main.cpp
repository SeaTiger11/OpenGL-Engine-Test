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
int pointLightCount = 0;
int directionalLightCount = 0;
int spotLightCount = 0;

//GLFW error logger
static void glfwError(int id, const char* description)
{
	std::cout << description << std::endl;
}

//Updates fragment shader to include an additional point light
void CreatePointLight(Shader& shader, glm::vec4 lightColour, glm::vec3 lightPos, float a, float b, float c) {
	std::string prefix = "pointLights[" + std::to_string(pointLightCount++);

	glUniform4f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightColour").c_str()), lightColour.x, lightColour.y, lightColour.z, lightColour.w);
	glUniform3f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightPos").c_str()), lightPos.x, lightPos.y, lightPos.z);
	glUniform1f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].a").c_str()), a);
	glUniform1f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].b").c_str()), b);
	glUniform1f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].c").c_str()), c);
}

//Updates fragment shader to include an additional directional light
void CreateDirectionalLight(Shader& shader, glm::vec4 lightColour, glm::vec3 lightDirection) {
	std::string prefix = "directionalLights[" + std::to_string(directionalLightCount++);

	glUniform4f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightColour").c_str()), lightColour.x, lightColour.y, lightColour.z, lightColour.w);
	glUniform3f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightDirection").c_str()), lightDirection.x, lightDirection.y, lightDirection.z);
}

//Updates fragment shader to include an additional spot light
void CreateSpotLight(Shader& shader, glm::vec4 lightColour, glm::vec3 lightPos, glm::vec3 lightDirection, float innerCone, float outerCone) {
	std::string prefix = "spotLights[" + std::to_string(spotLightCount++);

	glUniform4f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightColour").c_str()), lightColour.x, lightColour.y, lightColour.z, lightColour.w);
	glUniform3f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightPos").c_str()), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].lightDirection").c_str()), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform1f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].innerCone").c_str()), innerCone);
	glUniform1f(glGetUniformLocation(shader.ID, (prefix + (std::string)"].outerCone").c_str()), outerCone);
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
	//3 numbers for the position, 3 numbers for the rgb colours, 2 numbers for the texture cords, 3 numbers for vertex normals)
	GLfloat vertices[] =
	{ 
		-1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		 1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
		 1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
	};

	GLuint indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	//Cube to represent light source (just has coordinates)
	GLfloat lightVertices[] =
	{
		-0.1f, -0.1f,  0.1f,
		-0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f, -0.1f,
		 0.1f, -0.1f,  0.1f,
		-0.1f,  0.1f,  0.1f,
		-0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f, -0.1f,
		 0.1f,  0.1f,  0.1f
	};

	GLuint lightIndices[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};

	//Creating the window and then checking the process didn't fail
	GLFWwindow* window = glfwCreateWindow(width, height, "Demo Game", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Load glad to configure OpenGL
	gladLoadGL();

	//Set OpenGL viewport
	glViewport(0, 0, width, height);

	//Creates shader object with default.vert and default.frag shaders
	Shader shaderProgram("default.vert", "default.frag");

	//Generates vertex array object and binds it
	VAO VAO1;
	VAO1.Bind();

	//Generates vertex and element buffers and links them to vertices and indices
	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	//Links vertex buffer to vertex array attributes
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	//Unbinds all objects to prevent accidental modifications
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Shader for light cube
	Shader lightShader("light.vert", "light.frag");
	// Generates vertex array object and binds it
	VAO lightVAO;
	lightVAO.Bind();
	// Generates vertex buffer object and links it to the light vertices
	VBO lightVBO(lightVertices, sizeof(lightVertices));
	// Generates element buffer object and links it to the light indices
	EBO lightEBO(lightIndices, sizeof(lightIndices));
	// Links vertex buffer attributes such as coordinates and colours to the vertex array
	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	// Unbind all to prevent accidentally modifying them
	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();

	//Generates light and model data for the shaders
	glm::vec4 lightColour = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	//Updates light and model information for the light shader and the default shader
	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColour"), lightColour.x, lightColour.y, lightColour.z, lightColour.w);

	//Activates main shader and updates model information
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));

	//Adds lights and updates the fragment shader with the number of added lights
	CreateDirectionalLight(shaderProgram, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 0.0f));
	CreatePointLight(shaderProgram, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, -0.5f), 1.0f, 0.7f, 0.0f);
	CreateSpotLight(shaderProgram, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), lightPos, glm::vec3(0.0f, -1.0f, 0.0f), 0.95f, 0.90f);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "pointLightCount"), pointLightCount);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "directionalLightCount"), directionalLightCount);
	glUniform1i(glGetUniformLocation(shaderProgram.ID, "spotLightCount"), spotLightCount);
	
	//Generates texture
	Texture planksTex("planks.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
	planksTex.TexUnit(shaderProgram, "tex0", 0);
	Texture planksSpec("planksSpec.png", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE);
	planksSpec.TexUnit(shaderProgram, "tex1", 1);

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

		//Gets delta time to ensure that inputs aren't frame rate dependant
		auto now = std::chrono::steady_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count() / 1000000.0f;
		lastUpdate = now;

		//Updates inputs for the camera
		camera.Inputs(window, deltaTime);
		//Sets camera position and orientation
		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

		//Tell OpenGL what shaders to use
		shaderProgram.Activate();
		//Update camera position in fragment shader for use in specular lighting
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
		//Sends the camera matrix to the vertex shader
		camera.Matrix(shaderProgram, "camMatrix");

		//Binds the plank textures
		planksTex.Bind();
		planksSpec.Bind();
		//Bind the vertex array so OpenGL knows to use it
		VAO1.Bind();
		//Draw all the triangles
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		//Tells OpenGL what shaders to use
		lightShader.Activate();
		//Exports the camera matrix to the light cube's vertex shader
		camera.Matrix(lightShader, "camMatrix");
		//Bind the vertex array so OpenGL knows to use it
		lightVAO.Bind();
		//Draw all the triangles
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		//Swap the back and front buffers
		glfwSwapBuffers(window);

		//Handles all glfw events
		glfwPollEvents();
	}

	//Delete all created objects
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	planksTex.Delete();
	planksSpec.Delete();
	shaderProgram.Delete();
	lightVAO.Delete();
	lightVBO.Delete();
	lightEBO.Delete();
	lightShader.Delete();

	//Destroy window then terminate glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}