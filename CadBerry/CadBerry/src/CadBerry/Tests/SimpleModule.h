#pragma once
#include "CadBerry/Module/Module.h"
#include "SimpleViewport.h"

namespace CDB
{
	class ExampleModule : public Module
	{
	public:
		ExampleModule() : Module("Example module") {}
		virtual ~ExampleModule() override {}
		std::string Vps[2] = { "Example viewport 1", "Example viewport 2" };

		virtual std::string* GetViewportNames() override
		{
			NumViewports = 2;
			return Vps;
		}

		virtual Viewport* CreateViewport(std::string Name) override
		{
			return new TestViewport(Name);
		}

	};
}