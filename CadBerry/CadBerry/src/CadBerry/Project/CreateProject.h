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
		char ProjectName[64] = "New CadBerry Project";    //TODO: Change this to use std::string
		nfdchar_t* Path;    //TODO: Change this to use scoped_ptr, since right now it's being leaked
	};
}