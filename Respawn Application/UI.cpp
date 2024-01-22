#include "UI.h"

Canvas::Canvas(json accessor) {
	if (accessor.contains("worldSpace")) worldSpace = accessor["worldSpace"];
	if (accessor.contains("position")) position = glm::vec3(accessor["position"][0], accessor["position"][1], accessor["position"][2]);

	for (int i = 0; i < accessor["Images"].size(); i++) {
		json imageJson = accessor["Images"][i];
		Image image;

		if (imageJson.contains("x")) image.x = imageJson["x"];
		if (imageJson.contains("y")) image.y = imageJson["y"];

		if (imageJson.contains("width")) image.width = imageJson["width"];
		if (imageJson.contains("height")) image.height = imageJson["height"];

		if (imageJson.contains("unitType")) image.unitType = imageJson["unitType"];
		if (imageJson.contains("visible")) image.visible = imageJson["visible"];

		if (imageJson.contains("texture")) {
			std::string textureUrl = imageJson["texture"];
			bool skip = false;
			for (unsigned int j = 0; j < textureUrls.size(); j++) {
				if (textureUrls[j] == textureUrl) {
					image.texture = textures[j];
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture* newTexture = new Texture(textureUrl.c_str(), "image", textureCount++);
				textures.push_back(newTexture);
				image.texture = newTexture;
			}
		}
		else
			std::cout << "Missing texture url" << std::endl;

		std::vector<Vertex> vertices = {
			Vertex{
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				glm::vec2(0.0f, 0.0f)
			},
			Vertex{
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				glm::vec2(0.0f, 1.0f)
			},
			Vertex{
				glm::vec3(1.0f, 1.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				glm::vec2(1.0f, 1.0f)
			},
			Vertex{
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
				glm::vec2(1.0f, 0.0f)
			}
		};

		// Indices for vertices order
		std::vector<GLuint> indices =
		{
			0, 2, 1, // Upper triangle
			0, 3, 2 // Lower triangle
		};

		image.VAO.Bind();

		//Generates vertex and element buffers and links them to vertices and indices
		VBO VBO(vertices);
		EBO EBO(indices);

		//Links vertex buffer to vertex array attributes
		image.VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
		image.VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
		image.VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
		image.VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
		//Unbinds all objects to prevent accidental modifications
		image.VAO.Unbind();
		VBO.Unbind();
		EBO.Unbind();

		images.push_back(image);
	}
}

void Canvas::Draw(Shader& shader) {
	shader.Activate();

	for (int i = 0; i < images.size(); i++) {
		images[i].VAO.Bind();

		(*images[i].texture).TexUnit(shader, "image", textureCount);
		(*images[i].texture).Bind();

		float adjustment = 1.0f;
		if (images[i].unitType == 1) adjustment = cameraWidth / 100;
		if (images[i].unitType == 2) adjustment = cameraHeight / 100;

		glUniform3f(glGetUniformLocation(shader.ID, "position"), images[i].x * adjustment, images[i].y * adjustment, 0);
		glUniform3f(glGetUniformLocation(shader.ID, "scale"), images[i].width * adjustment, images[i].height * adjustment, 0);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
}