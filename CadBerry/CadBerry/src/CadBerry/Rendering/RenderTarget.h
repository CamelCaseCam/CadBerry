#pragma once
#include "CadBerry/Core.h"

#pragma warning(push, 0)
#include "imgui.h"
#pragma warning(pop)

namespace CDB
{
	/*
	Texture that the renderer renders to
	*/
	class CDBAPI RenderTarget
	{
	public:
		virtual ~RenderTarget() {}

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Draw(ImVec2 Location, ImVec2 Scale) = 0;

		//For testing, does not work
		virtual void SetAsBackground(ImVec2 Location, ImVec2 Scale) = 0;

		static RenderTarget* Create();
	};
}