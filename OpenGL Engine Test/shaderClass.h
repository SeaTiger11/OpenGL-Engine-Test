#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

//Gets file's contents as a string
std::string get_file_contents(const char* filename);

class Shader {
	public:
		//Reference ID
		GLuint ID;
		//Constructor
		Shader(const char* vertexFile, const char* fragmentFile);

		//Activates the shader program
		void Activate();
		//Deletes the shader program
		void Delete();

	private:
		//Checks for any errors when compiling or linking shaders
		void CompileErrors(unsigned int shader, const char* type);
};

#endif