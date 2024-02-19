#ifndef UI_CLASS_H
#define UI_CLASS_H

#include "GlobalVariables.h"

#include "Texture.h"
#include "VAO.h"
#include "EBO.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <vector>
#include <json/json.h>

using json = nlohmann::json;

struct Element {
	float x, y, width, height; //The position and scale of the element
	int unitType; //Should the units be measured in pixels, percentage of view width or percentage of view height
	bool visible = true; //A toggle for whether the element should be shown

	VAO VAO;
};

struct Image:Element {
	Texture* texture; //The texture to display as the element
	Image() {};
};

struct Button:Element {
	Texture* texture; //The texture to display as the element
	void (*buttonFunction)(void); //The function to call when the element is pressed
	Button() {};
};

//Will add text support to the game engine, but until then text won't be supported
/*
struct Text:Element {
	std::string text; //The text to write over the element
};
*/

class Canvas {
	public:
		Canvas(json accessor = NULL);
		void Draw(Shader& shader);

		bool visible = true;
	private:
		bool worldSpace = false;
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

		std::vector<Image> images;
		std::vector<Button> buttons;

		std::vector<std::string> textureUrls;
		std::vector<Texture*> textures;
};

#endif