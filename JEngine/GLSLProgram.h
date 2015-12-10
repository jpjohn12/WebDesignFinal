#pragma once

#include <string>
#include <GL/glew.h>

namespace JEngine {
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();
		void compileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
		void linkShaders();
		void addAttribute(const std::string& attributeName);
		void use();
		void unuse();
		GLint getUniformLocation(const std::string uniformName);

	private:
		void compileShader(const std::string filePath, GLuint id);

		int _numAttributes;
		GLuint _programID;
		GLuint _vertexShaderID;
		GLuint _fragmentShaderID;

	};

}