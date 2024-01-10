#include "Texture.h"

//Constructor
Texture::Texture(const char* image, GLenum texType, GLuint slot, GLenum format, GLenum pixelType) {
	type = texType;

	//Variables for image properties to be stored
	int imgWidth, imgHeight, numColourChannels;
	//By default stb loads image differently to openGL resulting in flipped image
	stbi_set_flip_vertically_on_load(true);
	//Turns image into byte array and updates image property variables
	unsigned char* imgBytes = stbi_load(image, &imgWidth, &imgHeight, &numColourChannels, 0); //Get image bytes

	//Generates a 2D texture
	glGenTextures(1, &ID);
	//Assigns texture to texture unit
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	//Binds texture
	glBindTexture(texType, ID);

	//Interpolates new pixels when scaling (may make image a bit blurry)
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Makes texture repeat as needed for the texture coordinates
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/*
	//Makes image not repeat and instead simply have a border colour
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//Generates a colour then sets it as the border colour for the texture
	float flatColour[] = { 1, 1, 1, 1 };
	glTexParameterfv(texType, GL_TEXTURE_BORDER_COLOR, flatColour);
	*/

	//Sets the texture data to the previously created image bytes
	glTexImage2D(texType, 0, GL_RGBA, imgWidth, imgHeight, 0, format, pixelType, imgBytes);
	//Generates image mipmaps (smaller versions of the same image)
	glGenerateMipmap(texType);

	//Delete the image bytes
	stbi_image_free(imgBytes);
	//Unbind the texture to prevent accidental modifications
	glBindTexture(texType, 0);
}

//Sets the texture variable ID in the fragment shader
void Texture::TexUnit(Shader& shader, const char* uniform, GLuint unit) {
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit); 
}

//Binds texture
void Texture::Bind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(type, ID);
}

//Unbinds texture
void Texture::Unbind() {
	glBindTexture(type, 0);
}

//Deletes texture
void Texture::Delete() {
	glDeleteTextures(1, &ID);
}