#ifndef SHADER_H
#define SHADER_H
#include "glad/glad.h" // include glad to get the required OpenGL headers
#include "GLFW/glfw3.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/vec3.hpp>

class Shader
{
public:
	// the program ID
	unsigned int ID;
	unsigned int vertex, fragment;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	void use();
	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

	void setVec3(const std::string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
};
#endif
