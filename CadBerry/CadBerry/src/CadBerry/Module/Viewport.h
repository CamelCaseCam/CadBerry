#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"
#include "CadBerry/Layer.h"
#include "CadBerry/Utils/memory.h"

namespace CDB
{
	class CDBAPI Viewport
	{
		friend class ViewportLayer;
	public:
		Viewport(std::string name) : Name(name) {}
		virtual ~Viewport();

		virtual void Start() {}
		virtual void Draw() {}
		virtual void GUIDraw() {}
		virtual void HeadlessInput();
		virtual void Update(float dt) {}
		virtual void OnClose() {}

		std::string Name;
	protected:
		bool IsOpen = true;
		bool Background = false;
	};

	class ViewportLayer : public Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnImGuiRender() override;
		virtual void Draw() override;
		~ViewportLayer();

		virtual void HeadlessInput() override;

		void AddViewport(Viewport* viewport);

		void SettingsWindow();

		std::vector<Viewport*> OpenViewports;

		bool OpenSettings = false;
	};
}