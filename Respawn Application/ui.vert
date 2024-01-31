#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColour;
layout (location = 3) in vec2 aTex;

//Outputs the texure coordinate for the fragment shader to use
out vec2 texCoord;

uniform vec3 position, scale;

void main() {
	texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;

	gl_Position = vec4(vec3(-1.0f, 1.0f, 0.0f) + aPos * scale * 2.0f + position, 1.0f);
}