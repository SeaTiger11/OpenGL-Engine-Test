#version 330 core
out vec4 FragColour;

//Inputs colour from the vertex shader
in vec3 colour;

//Inputs texture coordinate from the vertex shader
in vec2 texCord;

uniform sampler2D tex0;

void main() {
	//Sets the fragment colour based on the input with a constant alpha of 1
	//FragColour = vec4(colour, 1.0f);
	
	FragColour = texture(tex0, texCord);
}