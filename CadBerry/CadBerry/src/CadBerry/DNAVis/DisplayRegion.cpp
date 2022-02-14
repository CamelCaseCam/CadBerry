#include <cdbpch.h>
#include "DisplayColours.h"
#include "DisplayRegion.h"
#include "CadBerry/Rendering/Buffer.h"
#include "CadBerry/Rendering/RenderCommand.h"

#include "imgui.h"
#include "CadBerry/Rendering/Renderer.h"

namespace CDB
{
#define PointPercentage 0.1f
	DisplayRegion::DisplayRegion(float width, float height, float x, float y, int colourIdx, std::string Name) : Width(width), Height(height), X(x), 
		Y(y), ColourIdx(colourIdx)
	{
		this->Verts = VertexArray::Create();

		float ArrowOffset = 0.1f;
		if (width < 0.1f)
			ArrowOffset = width;

		float Vertices[5 * 7] = {
			x, y + (height/2), 0.1f,                                       Colours[colourIdx].w, Colours[colourIdx].x, Colours[colourIdx].y, Colours[colourIdx].z,
			x + (width - ArrowOffset), y + (height / 2), 0.1f,   Colours[colourIdx].w, Colours[colourIdx].x, Colours[colourIdx].y, Colours[colourIdx].z,
			x + width, y, 0.1f,                                            Colours[colourIdx].w, Colours[colourIdx].x, Colours[colourIdx].y, Colours[colourIdx].z,
			x + (width - ArrowOffset), y - (height / 2), 0.1f,   Colours[colourIdx].w, Colours[colourIdx].x, Colours[colourIdx].y, Colours[colourIdx].z,
			x, y - (height / 2), 0.1f,                                     Colours[colourIdx].w, Colours[colourIdx].x, Colours[colourIdx].y, Colours[colourIdx].z,
		};

		VertexBuffer* buffer = VertexBuffer::Create(sizeof(Vertices), Vertices);

		{
			BufferLayout layout = {
				{ShaderDataType::Float3, "pos"},
				{ShaderDataType::Float4, "colour"},
			};

			buffer->SetLayout(layout);
		}

		uint32_t indices[9] = { 0, 1, 2, 0, 2, 3, 0, 3, 4 };
		IndexBuffer* indexBuffer = IndexBuffer::Create(9, indices);
		this->Verts->AddVertexBuffer(buffer);
		this->Verts->SetIndexBuffer(indexBuffer);
	}

	void DisplayRegion::Draw()
	{
		CDB::Renderer::Submit(this->Verts.raw());
	}
}