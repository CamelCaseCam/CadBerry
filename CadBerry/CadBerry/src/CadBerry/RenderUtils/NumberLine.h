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
		int Intervals, TotalLength;
		float Scale, X, Y;
		scoped_ptr<VertexArray> Verts;
	};
}