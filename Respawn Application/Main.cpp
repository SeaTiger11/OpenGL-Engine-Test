#include "Model.h"
#include "Cubemap.h"
#include <chrono>

const unsigned int cameraWidth = 800, cameraHeight = 800;
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

	//Creating the window and then checking the process didn't fail
	GLFWwindow* window = glfwCreateWindow(cameraWidth, cameraHeight, "Demo Game", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Load glad to configure OpenGL
	gladLoadGL();

	//Set OpenGL viewport
	glViewport(0, 0, cameraWidth, cameraHeight);

	//Generates shader objects
	Shader defaultShader("default.vert", "default.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");


	//Activates main shader and updates model information
	defaultShader.Activate();

	//Adds lights and updates the fragment shader with the number of added lights
	CreateDirectionalLight(defaultShader, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 0.0f));
	//CreatePointLight(defaultShader, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec3(-0.5f, 0.5f, -0.5f), 1.0f, 0.7f, 0.0f);
	//CreateSpotLight(defaultShader, glm::vec4(5.0f, 5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 0.95f, 0.90f);
	glUniform1i(glGetUniformLocation(defaultShader.ID, "pointLightCount"), pointLightCount);
	glUniform1i(glGetUniformLocation(defaultShader.ID, "directionalLightCount"), directionalLightCount);
	glUniform1i(glGetUniformLocation(defaultShader.ID, "spotLightCount"), spotLightCount);

	//Enables depth buffer
	glEnable(GL_DEPTH_TEST);

	//Creates camera object
	Camera camera(cameraWidth, cameraHeight, glm::vec3(0.0f, 1.0f, 2.0f));

	//Time storer for calculating delta time
	std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();

	//Imports 3D models as model objects
	Model model("models/Test/scene.gltf", 1);

	Cubemap skybox("skybox", cameraWidth, cameraHeight);

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

		//Draws the imported 3d models
		model.Draw(defaultShader, camera);

		//Skybox is drawn last to improve performace as it will always fail depth checks against other objects
		skybox.Draw(skyboxShader, camera);

		//Swap the back and front buffers
		glfwSwapBuffers(window);

		//Handles all glfw events
		glfwPollEvents();
	}

	//Delete all created objects
	defaultShader.Delete();
	skyboxShader.Delete();

	//Destroy window then terminate glfw
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}