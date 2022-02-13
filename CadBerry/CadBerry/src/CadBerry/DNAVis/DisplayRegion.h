#pragma once
#include <cdbpch.h>
#include "CadBerry/Rendering/Renderer.h"
#include "CadBerry/Utils/memory.h"

namespace CDB
{
	class DisplayRegion
	{
	public:
		DisplayRegion(float width, float height, float x, float y, int colourIdx, std::string Name);

		void Draw();

	private:
		int Width, Height, X, Y, ColourIdx;
		std::string Name;
		scoped_ptr<VertexArray> Verts;
	};
}