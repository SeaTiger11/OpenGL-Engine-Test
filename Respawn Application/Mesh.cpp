#include "Mesh.h"

//Constructor
Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures) {
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	VAO.Bind();

	//Generates vertex and element buffers and links them to vertices and indices
	VBO VBO(vertices);
	EBO EBO(indices);

	//Links vertex buffer to vertex array attributes
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	//Unbinds all objects to prevent accidental modifications
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}

//Draws the mesh
void Mesh::Draw(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
) {
	shader.Activate();
	VAO.Bind();

	//Keeps track of how many of each type of texture are being used
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	//Adds and binds each texture
	for (int i = 0; i < textures.size(); i++) {
		std::string num;
		std::string type = textures[i].type;

		if (type == "diffuse")
			num = std::to_string(numDiffuse++);
		else if (type == "specular")
			num = std::to_string(numSpecular++);
		textures[i].TexUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	if (numDiffuse > 0)
		glUniform1i(glGetUniformLocation(shader.ID, "diffuseMissing"), 0);
	if (numSpecular > 0)
		glUniform1i(glGetUniformLocation(shader.ID, "specularMissing"), 0);

	//Update camera position in fragment shader for use in specular lighting
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.position.x, camera.position.y, camera.position.z);
	//Sends the camera matrix to the vertex shader
	camera.Matrix(shader, "camMatrix");

	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

	//Draws the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}