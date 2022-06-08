#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"
#include "CadBerry/Events/Event.h"
#include "LayerStack.h"
#include "CadBerry/Math.h"

namespace CDB
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		
		WindowProps(const std::string& title = "CadBerry Editor", unsigned int width = 1280, 
			unsigned int height = 720) : Title(title), Width(width), Height(height) {}
	};

	//window interface implemented per platform
	class CDBAPI Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual Vec2f GetMousePos() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
		static Window* CreateHeadless(const WindowProps& props = WindowProps());
		LayerStack m_LayerStack;
	};
}