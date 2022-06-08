#include <cdbpch.h>
#include "DisplayDNA.h"
#include "DisplayColours.h"

#include <CadBerry/Rendering/RenderCommand.h>
#include "CadBerry/RenderUtils/RichText.h"

#include "imgui.h"
#include <random>
#include "misc/cpp/imgui_stdlib.h"

#include "CadBerry/Log.h"

#ifdef CDB_PLATFORM_WINDOWS    //TODO: Update this once g++ supports format
#include <format>
#else
#include <spdlog/fmt/fmt.h>
#endif

namespace CDB
{
#undef max
#define DisplayRegionHeight 0.2f
#define MaxBasesPerScreen 1000.0f
#define DefaultTextColour "!#cccccc"

	std::string RGB2Hex(ImVec4 RGB)
	{
		#ifdef CDB_PLATFORM_WINDOWS    //TODO: Update this once g++ supports format
			return std::format("!#{:x}{:x}{:x}", (int)(255 * RGB.w), (int)(255 * RGB.x), (int)(255 * RGB.y));
		#else
			return fmt::format("!#{:x}{:x}{:x}", (int)(255 * RGB.w), (int)(255 * RGB.x), (int)(255 * RGB.y));
		#endif
	}

	DNAVisualization::DNAVisualization(std::string code, std::vector<Region> regions) : Regions(regions)
	{
		NumBases = code.length();
		InitLineShader();
		this->numberLine = NumberLine(5, code.size(), 0.96f, -0.975f, 0.0f);

		this->Code.reserve(code.size() + (Regions.size() * 2 * 8));

		BasesPerScreen = (code.size() < MaxBasesPerScreen ? code.size() - 1 : MaxBasesPerScreen) / 2;
		RegionTarget = RenderTarget::Create();
		this->DisplayRegions.reserve(regions.size());
		float StartPos = -0.975f;
		float LastEnd = 0.0f;
		for (Region& r : regions)    //Here we calculate positions
		{
			//Add to code
			this->Code += code.substr(LastEnd, r.Start - 1);
			int Colour = NextColour();
			this->Code += RGB2Hex(Colours[Colour]);
			this->Code += code.substr(r.Start - 1, r.End);

			//Just in case, push the default colour into the string
			this->Code += DefaultTextColour;

			StartPos += (((float)r.Start - 1) - LastEnd) * 2 * 0.95 / BasesPerScreen;
			float width = ((float)r.End - ((float)r.Start - 0.0625)) * 0.95;
			this->DisplayRegions.push_back({width/BasesPerScreen, DisplayRegionHeight, StartPos, 0.0f, Colour, r.Name});
			StartPos += width * 2 / BasesPerScreen;
			LastEnd = r.End;


		}
		this->Code += code.substr(LastEnd, code.length() - 1);


		//Now create the shader
		this->RegionShader = Shader::Create(
R"(#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Colour;
out vec3 OutPos;
out vec4 v_Colour;
void main()
{
	OutPos = a_Position;
	gl_Position = vec4(a_Position, 1.0);
	v_Colour = a_Colour;
})", 

R"(#version 330 core
layout(location = 0) out vec4 colour;
in vec3 OutPos;
in vec4 v_Colour;
void main()
{
	colour = v_Colour;
})");
	}

	void DNAVisualization::DrawCode(int Width, int Height)
	{
		ImGui::BeginChild("DNA code display window", ImVec2(Width, Height), true, ImGuiWindowFlags_HorizontalScrollbar);
		RichText(this->Code, 0, 0, 0.3, 1);
		ImGui::EndChild();
	}

	void DNAVisualization::DrawDiagram(int Width, int Height)
	{
		ImGui::BeginChild((uint64_t)this, ImVec2(Width, Height), true, ImGuiWindowFlags_HorizontalScrollbar);    //Use pointer to memory address to get unique id
		this->RegionShader->Bind();

		CDB::Renderer::BeginScene(RegionTarget.raw());
		CDB::RenderCommand::SetClearColour({ 0.9, 0.9, 0.9, 1.0 });
		CDB::RenderCommand::Clear();
		
		for (DisplayRegion& dr : this->DisplayRegions)
			dr.Draw();

		LineShader->Bind();
		this->numberLine.Draw(false);

		CDB::Renderer::EndScene();
		RegionTarget->Draw(ImVec2(this->NumBases * Width/BasesPerScreen, Height), ImVec2(1.0, 1.0));

		LineShader->Unbind();    //We shouldn't leave LineShader bound
		ImGui::EndChild();
	}
}