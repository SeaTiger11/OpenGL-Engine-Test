#include "shaderClass.h"

//Gets file's contents as a string
std::string get_file_contents(const char* filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw (errno);
}

//Shader program constructor from vertex and fragment shader
Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	//Convert the shaders into strings then into character arrays
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//Creates shader objects, sets shader code then compiles to machine code
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//Create shader program then attach the shaders to the program and link them all
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	//Delete the now useless shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

//Activates shader program
void Shader::Activate() {
	glUseProgram(ID);
}

//Deletes shader program
void Shader::Delete() {
	glDeleteProgram(ID);
}