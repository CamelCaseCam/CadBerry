#include <cdbpch.h>
#include "OpenGLContext.h"
#include "CadBerry/Log.h"

#include <glad/glad.h>

namespace CDB
{
	OpenGLContext::OpenGLContext(GLFWwindow* window) : WindowHandle(window)
	{
		CDB_EditorAssert(WindowHandle, "WindowHandle is null")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CDB_EditorAssert(status, "Failed to load Glad")

		CDB_BuildInfo("Detected {0} GPU", glGetString(GL_VENDOR));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(WindowHandle);
	}
}