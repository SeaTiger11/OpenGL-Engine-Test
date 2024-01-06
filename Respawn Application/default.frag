#version 330 core
out vec4 FragColour;

//Inputs colour from the vertex shader
in vec3 colour;

//Inputs texture coordinate from the vertex shader
in vec2 texCord;

//Uniform texture variable
uniform sampler2D tex;

void main() {
	//Sets the fragment colour based on the input with a constant alpha of 1
	//FragColour = vec4(colour, 1.0f);
	
	//Sets fragment colour based off the texture and the texture coordinate 
	FragColour = texture(tex, texCord);
}