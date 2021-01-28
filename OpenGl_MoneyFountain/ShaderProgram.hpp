#pragma once
#include <initializer_list>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <glad/glad.h>

class ShaderProgram 
{
private:
	unsigned int ID;

public:

	void Init(const char* vertexShaderPath, const char* fragmentShaderPath) 
	{
		std::ifstream vertexFile, fragmentFile;
		std::stringstream vertexStream, fragmentStream;
		std::string vertexCode, fragmentCode;

		//ensure ifstream objects can throw exceptions:
		vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		//read files
		vertexFile.open(vertexShaderPath);
		fragmentFile.open(fragmentShaderPath);

		vertexStream << vertexFile.rdbuf();
		fragmentStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();

		//compile shaders
		int success;
		char infoLog[512];
		const char* vCode = vertexCode.c_str();
		const char* fCode = fragmentCode.c_str();
		unsigned int vertex, fragment;

		//compile vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vCode, NULL);
		glCompileShader(vertex);

		//check if vertex shader is successfully compiled
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) 
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			throw std::runtime_error(infoLog);
		}

		//compile fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fCode, NULL);
		glCompileShader(fragment);

		//check if fragment shader is successfully compiled
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) 
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			throw std::runtime_error(infoLog);
		}

		//link shaders to program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);

		//check if the program is successfully linked
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) 
		{
			glGetProgramInfoLog(ID, 512, NULL,infoLog);
			throw std::runtime_error(infoLog);
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	};

	//use shader program
	void use() 
	{
		glUseProgram(ID);
	}
	
	//get shader program ID
	unsigned int getID() 
	{
		return ID;
	}

	//get uniform variable location
	auto getUniformLocation(const char* varName) 
	{
		return glGetUniformLocation(ID, varName);
	}
};