#pragma once
#include "CadBerry/Rendering/RenderTarget.h"
#include <CadBerry/Rendering/Renderer.h>
#include "CadBerry/Utils/memory.h"
#include "DisplayRegion.h"
#include "CadBerry/Rendering/Shader.h"
#include "CadBerry/RenderUtils/NumberLine.h"

#include "imgui.h"
#include <cdbpch.h>

namespace CDB
{
	struct Region
	{
		Region(int start, int end, std::string name) : Start(start), End(end), Name(name) {}
		int Start, End;
		std::string Name;
	};

	class DNAVisualization
	{
	public:
		DNAVisualization() {}
		DNAVisualization(std::string code, std::vector<Region> regions);

		void DrawCode(int Width, int Height);
		void DrawDiagram(int Width, int Height);

	private:
		int NumBases;
		float BasesPerScreen;
		std::string Code;
		std::vector<Region> Regions;

		scoped_ptr<RenderTarget> RegionTarget;
		scoped_ptr<Shader> RegionShader;


		std::vector<DisplayRegion> DisplayRegions;
		NumberLine numberLine;
	};
}