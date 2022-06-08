#pragma once
#ifdef CDB_DEBUG
#include <cdbpch.h>
#include "CadBerry/Module/Viewport.h"
#include "imgui.h"

namespace CDB
{
	class TestViewport : public Viewport
	{
	public:
		TestViewport(std::string name) : Viewport(name) {}

		virtual void Start() override {}
		virtual void Draw() override
		{
			ImGui::Text("Test viewport!!!");
		}
		virtual void OnClose() override {}

		~TestViewport() override {}
	};
}


#endif //CDB_DEBUG