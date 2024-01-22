#version 330 core
out vec4 FragColour;

//Inputs texture coordinate from the vertex shader
in vec2 texCoord;

uniform sampler2D image;

void main() {
	FragColour = texture(image, texCoord);
}