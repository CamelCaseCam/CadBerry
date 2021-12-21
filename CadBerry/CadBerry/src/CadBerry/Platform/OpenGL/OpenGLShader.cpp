#include <cdbpch.h>
#include "OpenGLShader.h"
#include "CadBerry/Log.h"

#include <glad/glad.h>

namespace CDB
{
	OpenGLShader::OpenGLShader(const std::string& VertSrc, const std::string& FragSrc)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = VertSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			CDB_EditorFatal("Failed to compile vertex shader with error message:\n{0}", infoLog.data());
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = FragSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			CDB_EditorFatal("Failed to compile fragment shader with error message:\n{0}", infoLog.data());
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(RendererID, vertexShader);
		glAttachShader(RendererID, fragmentShader);

		// Link our program
		glLinkProgram(RendererID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(RendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			CDB_EditorFatal("OpenGL program linking failed with error message:\n{0}", infoLog.data());
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(RendererID, vertexShader);
		glDetachShader(RendererID, fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(RendererID);    //Delete the program
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}
}