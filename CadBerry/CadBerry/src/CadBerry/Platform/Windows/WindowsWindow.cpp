#include "cdbpch.h"

#include "WindowsWindow.h"
#include "CadBerry/Log.h"
#include "CadBerry/Application.h"
#include "WindowsInput.h"
#include "CadBerry/Platform/OpenGL/OpenGLContext.h"

//I'm including these event types because they'll be handled by the window, but build events will be dispatched by the gui system when the 
//user clicks something along the lines of Project->Build
#include "CadBerry/Events/ApplicationEvent.h"
#include "CadBerry/Events/KeyEvent.h"
#include "CadBerry/Events/MouseEvent.h"

#include "CadBerry/Rendering/RenderCommand.h"

namespace CDB
{
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int code, const char* message)    //Prints GLFW errors to the console
	{
		CDB_EditorError("GLFW Error {0}: {1}", code, message);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown(true);
	}
	
	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		CDB_EditorInfo("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();

			CDB_EditorAssert(success, "Could not initialize GLFW")
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwShowWindow(m_Window);
		
		m_GraphicsContext = new OpenGLContext(m_Window);
		m_GraphicsContext->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//set glfw callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
		{
			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			Data.Width = width;
			Data.Height = height;
			
			WindowResizeEvent e(width, height);
			Data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& Data =*(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent e;
			Data.EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					Data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					Data.EventCallback(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					Data.EventCallback(e);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);

			//Adding Mouse x and y position to the mouse pressed and released events is barely any work for me and will come in handy later
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(Data.MouseX, Data.MouseY, button);
					Data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
					MouseButtonReleasedEvent e(Data.MouseX, Data.MouseY, button);
					Data.EventCallback(e);
					break;
				}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double XOffset, double YOffset)
		{
			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent e((float)XOffset, (float)YOffset);
			Data.EventCallback(e);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
		{
			WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
			Data.MouseX = (float)x;
			Data.MouseY = (float)y;
			MouseMovedEvent e(Data.MouseX, Data.MouseY);
			Data.EventCallback(e);
		});
		Input::SetInstance(new WindowsInput());
	}

	void WindowsWindow::Shutdown(bool PreserveGLFW)
	{
		glfwDestroyWindow(m_Window);

		if (!PreserveGLFW)
		{
			glfwTerminate();
			s_GLFWInitialized = false;
		}
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_GraphicsContext->SwapBuffers();
		RenderCommand::SetClearColour({ 0.1, 0.1, 0.1, 1.0 });
		RenderCommand::Clear();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = true;
	}

	bool WindowsWindow::IsVSync() const	
	{
		return m_Data.VSync;
	}
}