#pragma once
#include <cdbpch.h>
#include "CadBerry/Rendering/Renderer.h"
#include "CadBerry/Utils/memory.h"

namespace CDB
{
	struct Point2D
	{
		float x, y;
	};

	struct CDBColour
	{
		float r, g, b, a;
	};

	class Polygon
	{
	public:
		Polygon(std::vector<Point2D> Points, CDBColour PolygonColour);

		void Draw();

	private:
		scoped_ptr<VertexArray> Verts;
	};

	//Converts ImVec4 to ImU32
	void ImGuiPolygon(std::vector<ImVec2> Points, ImVec4 PolygonColour);
	void ImGuiPolygon(std::vector<ImVec2> Points, ImU32 PolygonColour);

	IndexBuffer* PointsToTriBuffer(int NumPoints);
	char FloatCol2Byte(float col);
}