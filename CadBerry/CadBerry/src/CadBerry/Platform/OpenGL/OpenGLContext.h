#pragma once
#include "CadBerry/Rendering/GraphicsContext.h"
#include "CadBerry/Window.h"

#include <GLFW/glfw3.h>

namespace CDB
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* WindowHandle;
	};
}