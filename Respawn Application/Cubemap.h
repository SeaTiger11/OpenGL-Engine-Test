#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include "VAO.h"
#include "EBO.h"
#include "Camera.h"
#include <stb/stb_image.h>
#include <iostream>

const float cubemapVertices[] =
{
	//Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

const unsigned int cubemapIndices[] =
{
	//Right
	1, 2, 6,
	6, 5, 1,
	//Left
	0, 4, 7,
	7, 3, 0,
	//Top
	4, 5, 6,
	6, 7, 4,
	//Bottom
	0, 3, 2,
	2, 1, 0,
	//Back
	0, 1, 5,
	5, 4, 0,
	//Front
	3, 7, 6,
	6, 2, 3
};


class Cubemap {
	public:
		//Constructor
		Cubemap(std::string directory, unsigned int cameraWidth, unsigned int cameraHeight);

		//Draws the cubemap
		void Draw(Shader& shader, Camera& camera);
	private:
		unsigned int VAO, VBO, EBO, cubemapTexture, cameraWidth, cameraHeight;
};

#endif