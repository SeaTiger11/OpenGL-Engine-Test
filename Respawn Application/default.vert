#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColour;
layout (location = 3) in vec2 aTex;

//Outputs the current position for the fragment shader to use
out vec3 crntPos;
//Outputs the normal for the fragment shader to use
out vec3 Normal;
//Outputs the colour for the fragment shader to use
out vec4 colour;
//Outputs the texure coordinate for the fragment shader to use
out vec2 texCoord;

//Uniform matrix variable
uniform mat4 camMatrix;

//Uniform model variable
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
	//The current position of the vertex
	crntPos = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
	//Sets the normal variable from the vertex data
	Normal = aNormal;
	//Sets the colour variable from the vertex data
	colour = aColour;
	//Sets the texture coordinate from the vertex data
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

	//Sets the vertex position based on the vertex data and the uniform scale variable
	gl_Position = camMatrix * vec4(crntPos, 1.0);
}