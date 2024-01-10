#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

//Outputs the colour for the fragment shader to use
out vec3 colour;

//Outputs the texure coordinate for the fragment shader to use
out vec2 texCord;

out vec3 Normal;
out vec3 crntPos;

//Uniform matrix variable
uniform mat4 camMatrix;

//Uniform model variable
uniform mat4 model;

void main() {
	//The current position of the vertex
	crntPos = vec3(model * vec4(aPos, 1.0f));

	//Sets the vertex position based on the vertex data and the uniform scale variable
	gl_Position = camMatrix * vec4(crntPos, 1.0);
	//Sets the colour variable from the vertex data
	colour = aColour;
	//Sets the texture coordinate from the vertex data
	texCord = aTex;
	//Sets the normal variable from the vertex data
	Normal = aNormal;
}