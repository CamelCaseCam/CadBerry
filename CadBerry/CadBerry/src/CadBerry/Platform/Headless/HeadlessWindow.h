#pragma once

#include "CadBerry/Window.h"
#include "CadBerry/Log.h"

namespace CDB
{
	class HeadlessWindow : public Window
	{
	public:
		HeadlessWindow(const WindowProps& props);
		virtual ~HeadlessWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return 0; }
		inline unsigned int GetHeight() const override { return 0; }
		inline Vec2f GetMousePos() const override { return Vec2f(0, 0); }

		inline void SetEventCallback(const EventCallbackFn& callback) override { CDB_EditorError("Headless mode doesn't support events"); }
		void SetVSync(bool enabled) override {}
		bool IsVSync() const override { return false; }

		inline virtual void* GetNativeWindow() const { CDB_EditorError("Headless mode doesn't use a native window"); return nullptr; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown(bool PreserveGLFW = false);
	};
}