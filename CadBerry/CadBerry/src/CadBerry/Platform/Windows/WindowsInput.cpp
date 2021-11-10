#include <cdbpch.h>
#include "WindowsInput.h"
#include "CadBerry/Application.h"
#include "WindowsWindow.h"

#include <GLFW/glfw3.h>


namespace CDB
{
	Input* Input::s_Instance = nullptr;

	bool WindowsInput::IsKeyPressedImpl(int KeyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, KeyCode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int MouseButton)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, MouseButton);

		return state == GLFW_PRESS;
	}

	Vec2f WindowsInput::GetMousePosImpl()
	{
		return Application::Get().GetWindow().GetMousePos();
	}
}