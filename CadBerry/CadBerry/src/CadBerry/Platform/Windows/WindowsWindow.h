#pragma once

#include "CadBerry/Window.h"
#include "CadBerry/Math.h"
#include "CadBerry/Rendering/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace CDB
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }
		inline Vec2f GetMousePos() const override { return Vec2f(m_Data.MouseX, m_Data.MouseY); }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown(bool PreserveGLFW = false);

		GLFWwindow* m_Window;
		GraphicsContext* m_GraphicsContext;

		struct WindowData
		{
			bool VSync;
			unsigned int Width, Height;
			float MouseY, MouseX;
			std::string Title;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}