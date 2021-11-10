#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"
#include "Viewport.h"

namespace CDB
{
	class CDBAPI Module
	{
	public:
		int NumViewports;
		std::string ModuleName;
		
		virtual std::string* GetViewportNames() = 0;
		virtual Viewport* CreateViewport(std::string Name) = 0;
	protected:
		Module(std::string name) : ModuleName(name) {}
		virtual ~Module() {}
	};
}