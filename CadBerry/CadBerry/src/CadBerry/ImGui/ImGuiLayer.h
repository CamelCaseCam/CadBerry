#pragma once

#include "CadBerry/Layer.h"
#include "CadBerry/Core.h"

#include "imgui.h"

#include "CadBerry/Events/ApplicationEvent.h"
#include "CadBerry/Events/KeyEvent.h"
#include "CadBerry/Events/MouseEvent.h"

namespace CDB
{
	class CDBAPI ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		ImGuiContext* Context;
	private:
		float m_Time = 0.0f;
	};
}