#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"
#include "Viewport.h"

#include "imgui.h"

namespace CDB
{
	class CDBAPI Module
	{
	public:
		int NumViewports;
		std::string ModuleName;
		
		virtual std::string* GetViewportNames() = 0;
		virtual Viewport* CreateViewport(std::string Name) = 0;
		virtual void InitImGui(ImGuiContext* Context) {}
	protected:
		Module(std::string name) : ModuleName(name) {}
		virtual ~Module() {}
	};
}

#define UseImGui virtual void InitImGui(ImGuiContext* Context) {ImGui::SetCurrentContext(Context);}