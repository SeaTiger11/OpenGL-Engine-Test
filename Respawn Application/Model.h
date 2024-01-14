#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.h>
#include "Mesh.h"

using json = nlohmann::json;

class Model {
	public:
		//Constructor
		Model(const char* file, unsigned int startingNode);

		//Draws all meshes in the imported 3d model
		void Draw(Shader& shader, Camera& camera);

	private:
		const char* file;
		std::vector<unsigned char> data;
		json JSON;

		//Data for each of the meshes in the imported model
		std::vector<Mesh> meshes;
		std::vector<glm::vec3> translationsMeshes;
		std::vector<glm::quat> rotationsMeshes;
		std::vector<glm::vec3> scalesMeshes;
		std::vector<glm::mat4> matricesMeshes;

		//The textures used by the meshes
		std::vector<std::string> loadedTexName;
		std::vector<Texture> loadedTex;

		//Loads the given mesh from the model
		void LoadMesh(unsigned int indMesh);

		//Traverses starting at the given parent and rendering it and all it's children
		void TraverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

		//Gets relevant data from the imported model
		std::vector<unsigned char> GetData();
		std::vector<float> GetFloats(json accessor);
		std::vector<GLuint> GetIndices(json accessor);
		std::vector<Texture> GetTextures();

		//Creates vertex objects from given data
		std::vector<Vertex> AssembleVertices(
			unsigned int indMes,
			std::vector<glm::vec3> positions,
			std::vector<glm::vec3> normals,
			std::vector<glm::vec2> texUVs
		);

		//Converts given floats into vectors
		std::vector<glm::vec2> GroupFloatsVec2(std::vector<float> floatVec);
		std::vector<glm::vec3> GroupFloatsVec3(std::vector<float> floatVec);
		std::vector<glm::vec4> GroupFloatsVec4(std::vector<float> floatVec);
};

#endif