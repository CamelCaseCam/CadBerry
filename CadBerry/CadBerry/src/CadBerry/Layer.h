#pragma once
#include "CadBerry/Core.h"
#include "CadBerry/Events/Event.h"

namespace CDB
{
	class CDBAPI Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}
		virtual void OnImGuiRender() {}
		virtual void Draw() {}

		virtual void HeadlessInput();

		inline const std::string& GetName() const { return m_Name; }
	private:
		std::string m_Name;
	};
}