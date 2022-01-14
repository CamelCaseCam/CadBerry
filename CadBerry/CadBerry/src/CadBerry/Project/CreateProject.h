#pragma once
#include "CadBerry/Layer.h"
#include "Project.h"

#include <nfd.h>

namespace CDB
{
	class ProjectCreationLayer : public Layer
	{
	public:
		ProjectCreationLayer(std::vector<ProjInfo> projects) : Projects(projects) {}
		virtual void OnImGuiRender() override;
		virtual void OnAttach() override;

	private:
		char ProjectName[64] = "New CadBerry Project";    //TODO: Change this to use std::string
		nfdchar_t* Path;    //TODO: Change this to use scoped_ptr, since right now it's being leaked
		std::vector<ProjInfo> Projects;
		bool CreateProject = false;

		void CreateNewProject();
	};
}