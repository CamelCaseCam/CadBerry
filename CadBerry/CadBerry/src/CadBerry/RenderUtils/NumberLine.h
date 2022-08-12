#pragma once
#include "CadBerry/Utils/memory.h"
#include <CadBerry/Rendering/Renderer.h>
#include "CadBerry/Rendering/Shader.h"

namespace CDB
{
	extern Shader* LineShader;
	void InitLineShader();

	class NumberLine
	{
	public:
		NumberLine() {}
		NumberLine(int intervals, int TotalLength, float scale, float x, float y);

		//Will bind the LineShader if BindShader isn't set to false
		void Draw(bool BindShader = true);
	private:
		int Intervals = 0, TotalLength = 0;
		float Scale = 0, X = 0, Y = 0;
		scoped_ptr<VertexArray> Verts;
	};
}