#pragma once
#include "CadBerry/Layer.h"
#include <nfd.h>

namespace CDB
{
	class ProjectCreationLayer : public Layer
	{
	public:
		virtual void OnImGuiRender() override;
		virtual void OnAttach() override;

	private:
		char ProjectName[64] = "New CadBerry Project";
		nfdchar_t* Path;
	};
}