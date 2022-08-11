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
<<<<<<< HEAD

		CDB_BuildInfo("Detected {0} GPU", glGetString(GL_VENDOR));
=======
>>>>>>> 6bb4493681bd7d96fea1812202eb7077be127cf8
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(WindowHandle);
		glClearColor(0, 0, 0, 1);    //TEMP
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}