#include <cdbpch.h>
#include "Utils.h"
#include "CadBerry/Log.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace CDB
{
	//Draw a polygon at these points with this colour
	Polygon::Polygon(std::vector<Point2D> Points, CDBColour PolygonColour)
	{
		if (Points.size() < 3)
		{
			/*If there aren't enough points to make a polygon, we have to throw a fatal error. The other options are 
			1. Ignore it and hope the draw method is never called (vertex array will be uninitialized)
			2. Use delete this to kill the polygon. This is even worse, since we can have read access violations and segfaults if it was 
				stack-allocated
			*/
			CDB_EditorFatal("Polygon class constructor expected at least 3 points, recieved {0}. ", Points.size());
			return;
		}

		float* vertices = new float[7 * Points.size()];
		for (int i = 0; i < 7 * Points.size(); i += 7)    //Populate vertex array
		{
			vertices[i] = Points[i].x;
			vertices[i + 1] = Points[i].y;
			vertices[i + 2] = 0.1f;
			vertices[i + 3] = PolygonColour.r;
			vertices[i + 4] = PolygonColour.g;
			vertices[i + 5] = PolygonColour.b;
			vertices[i + 6] = PolygonColour.a;
		}
		VertexBuffer* buffer = VertexBuffer::Create(sizeof(float) * 7 * Points.size(), vertices);

		{
			BufferLayout layout = {
				{ShaderDataType::Float3, "pos"},
				{ShaderDataType::Float4, "colour"},
			};

			buffer->SetLayout(layout);
		}

		//Get the index buffer
		IndexBuffer* indexBuffer = PointsToTriBuffer(Points.size());

		this->Verts->AddVertexBuffer(buffer);
		this->Verts->SetIndexBuffer(indexBuffer);

		delete[] vertices;    //TODO: Make it so scoped_ptr can handle arrays
	}

	void Polygon::Draw()
	{
		CDB::Renderer::Submit(this->Verts.raw());
	}

	
	void ImGuiPolygon(std::vector<ImVec2> Points, ImVec4 PolygonColour)
	{
		char Colour[4];    //Byte array
		Colour[0] = FloatCol2Byte(PolygonColour.w);
		Colour[1] = FloatCol2Byte(PolygonColour.x);
		Colour[2] = FloatCol2Byte(PolygonColour.y);
		Colour[3] = FloatCol2Byte(PolygonColour.z);
		ImU32 colour = *(ImU32*)Colour;    //Convert the bytes to ImU32
		ImGuiPolygon(Points, colour);
	}

	inline ImVec2 operator+(ImVec2 v1, ImVec2 v2)
	{
		return ImVec2(v1.x + v2.x, v1.y + v2.y);
	}

	void ImGuiPolygon(std::vector<ImVec2> Points, ImU32 PolygonColour)
	{
		if (Points.size() < 3)
		{
			CDB_EditorError("ImGuiPolygon expected at least three points");
			return;
		}
		ImVec2 MinPt;
		ImVec2 MaxPt;
		for (ImVec2 pt : Points)
		{
			if (pt.x < MinPt.x)
				MinPt.x = pt.x;
			else if (pt.x > MaxPt.x)
				MaxPt.x = pt.x;

			if (pt.y < MinPt.y)
				MinPt.y = pt.y;
			else if (pt.y > MaxPt.y)
				MaxPt.y = pt.y;
		}

		for (size_t i = 2; i < Points.size(); ++i)
		{
			ImGui::GetCurrentWindow()->DrawList->AddTriangleFilled(ImGui::GetCursorScreenPos() + Points[0], ImGui::GetCursorScreenPos() + Points[i - 1], ImGui::GetCursorScreenPos() + Points[i], PolygonColour);
		}
		ImGui::Dummy(ImVec2(MaxPt.x - MinPt.x, MaxPt.y - MinPt.y));
	}

	IndexBuffer* PointsToTriBuffer(int NumPoints)
	{
		if (NumPoints < 3)
		{
			CDB_BuildError("PointsToTriBuffer expected at least three points");
			return nullptr;
		}
		uint32_t* Indices = new uint32_t[(NumPoints - 2) * 3];
		int IndexIndex = 0;    //The place we're inserting the indices
		for (int i = 2; i < NumPoints; ++i)
		{
			Indices[IndexIndex] = 0;
			Indices[IndexIndex + 1] = i - 1;
			Indices[IndexIndex + 2] = i;
			IndexIndex += 3;
		}

		IndexBuffer* indexBuffer = IndexBuffer::Create((NumPoints - 2) * 3, Indices);
		delete[] Indices;

		return indexBuffer;
	}

	char FloatCol2Byte(float col)
	{
		if (col > 1.0f)
			return 255;

		return (char)(col * 255);
	}
}