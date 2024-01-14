#include "Model.h"

//Constructor
Model::Model(const char* file, unsigned int startingNode) {
	//Gets json data from the provided link to the 3d model
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	//Stores file url and 3d model data
	Model::file = file;
	data = GetData();

	//Renders initial mesh then continues through each child mesh
	TraverseNode(startingNode);
}

//Draws all meshes in the imported 3d model
void Model::Draw(Shader& shader, Camera& camera) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i], translationsMeshes[i], rotationsMeshes[i], scalesMeshes[i]);
	}
}

//Loads the given mesh from the model
void Model::LoadMesh(unsigned int indMesh) {
	//Gets indexs of vertex positions, normals, texture coordinates and the indices
	unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

	//Gets relevant data and converts them into the appropriate vectors
	std::vector<float> posVec = GetFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = GroupFloatsVec3(posVec);
	std::vector<float> normalVec = GetFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = GroupFloatsVec3(normalVec);

	std::vector<float> texVec = GetFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> texUVs = GroupFloatsVec2(texVec);

	//Creates vertexs from given data
	std::vector<Vertex> vertices = AssembleVertices(indMesh, positions, normals, texUVs);
	//Gets indices and textures
	std::vector<GLuint> indices = GetIndices(JSON["accessors"][indAccInd]);
	std::vector<Texture> textures = GetTextures();

	//Creates a mesh object from the vertices, indices and textures and stores them into the meshes vector
	meshes.push_back(Mesh(vertices, indices, textures));
}

//Traverses starting at the given parent and rendering it and all it's children
void Model::TraverseNode(unsigned int nextNode, glm::mat4 matrix) {
	//Gets mesh data based on the given index
	json node =  JSON["nodes"][nextNode];

	//Gets and formats the data for the mesh's transformations
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end()) {
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = node["translation"][i];
		translation = glm::make_vec3(transValues);
	}

	glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	glm::quat rotationForMatrix = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end()) {
		float rotValues[4] = {
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2],
			node["rotation"][3]
		};
		float rotValuesForMatrix[4] = {
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
		rotationForMatrix = glm::make_quat(rotValuesForMatrix);
	}

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end()) {
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = node["scale"][i];
		scale = glm::make_vec3(scaleValues);
	}

	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end()) {
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = node["matrix"][i];
		matNode = glm::make_mat4(matValues);
	}
	
	//Converts transformations into matrixs to find the overall matrix
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotationForMatrix);
	sca = glm::scale(sca, scale);

	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	//Store transformation data (not matrix versions) and overall matrix
	if (node.find("mesh") != node.end()) {
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);

		LoadMesh(node["mesh"]);
	}

	//If the given mesh contains any children, repeat the process for all the subsequent children
	if (node.find("children") != node.end()) {
		for (unsigned int i = 0; i < node["children"].size(); i++)
			TraverseNode(node["children"][i], matNextNode);
	}
}

//Gets model data as a character array
std::vector<unsigned char> Model::GetData() {
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

//Gets float data from the given accessor
std::vector<float> Model::GetFloats(json accessor) {
	std::vector<float> floatVec;

	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int numPerVert;
	if (type == "SCALER") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALER, VEC2, VEC3 or VEC4)");

	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i) {
		unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

//Gets indices data from the accessor formating it based on it's component type
std::vector<GLuint> Model::GetIndices(json accessor) {
	std::vector<GLuint> indices;

	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125) {
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i) {
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123) {
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i) {
			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122) {
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i) {
			unsigned char bytes[] = { data[i++], data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}
	return indices;
}

//Gets texture data and creates texture objects from it
std::vector<Texture> Model::GetTextures() {
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	for (unsigned int i = 0; i < JSON["images"].size(); i++) {
		std::string texPath = JSON["images"][i]["uri"];

		bool skip = false;
		for (unsigned int j = 0; j < loadedTexName.size(); j++) {
			if (loadedTexName[j] == texPath) {
				textures.push_back(loadedTex[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			if (texPath.find("baseColor") != std::string::npos) {
				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size());
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexName.push_back(texPath);
			}
			else if (texPath.find("metallicRoughness") != std::string::npos) {
				Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTex.size());
				textures.push_back(specular);
				loadedTex.push_back(specular);
				loadedTexName.push_back(texPath);
			}
		}
	}

	return textures;
}

//Creates vertex objects from given data
std::vector<Vertex> Model::AssembleVertices(
	unsigned int indMesh,
	std::vector<glm::vec3> positions, 
	std::vector<glm::vec3> normals, 
	std::vector<glm::vec2> texUVs
) {
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++) {
		if (JSON["images"].size() == 0) {
			unsigned int materialInd = JSON["meshes"][indMesh]["primitives"][0]["material"];
			json colourData = JSON["materials"][materialInd]["pbrMetallicRoughness"]["baseColorFactor"];
			vertices.push_back
			(
				Vertex{
					positions[i],
					normals[i],
					glm::vec4(colourData[0], colourData[1], colourData[2], colourData[3]),
					texUVs[i]
				}
			);
		}
		else {
			vertices.push_back
			(
				Vertex{
					positions[i],
					normals[i],
					glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
					texUVs[i]
				}
			);
		}
	}
	return vertices;
}

//Converts given floats into a vec2
std::vector<glm::vec2> Model::GroupFloatsVec2(std::vector<float> floatVec) {
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i) {
		vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	}
	return vectors;
}

//Converts given floats into a vec3
std::vector<glm::vec3> Model::GroupFloatsVec3(std::vector<float> floatVec) {
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i) {
		vectors.push_back(glm::vec3(-floatVec[i++], -floatVec[i++], -floatVec[i++]));
	}
	return vectors;
}

//Converts given floats into a vec4
std::vector<glm::vec4> Model::GroupFloatsVec4(std::vector<float> floatVec) {
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i) {
		vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}