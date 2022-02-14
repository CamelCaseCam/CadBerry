#include <cdbpch.h>
#include "NumberLine.h"
#include "CadBerry/Rendering/Buffer.h"
#include "CadBerry/Rendering/RenderCommand.h"

#include "imgui.h"
#include "CadBerry/Rendering/Renderer.h"

namespace CDB
{
	Shader* LineShader = nullptr;
	void InitLineShader()
	{
		if (LineShader == nullptr)
		{
			LineShader = Shader::Create(
				R"(#version 330 core
layout(location = 0) in vec2 a_Position;
void main()
{
	gl_Position = vec4(a_Position, 0.0, 1.0);
})",

R"(#version 330 core
layout(location = 0) out vec4 colour;
void main()
{
	colour = vec4(0.0, 0.0, 0.0, 1.0);
})");
		}
	}

#define EndPointSize 0.2f
#define IntervalSize 0.1f
	NumberLine::NumberLine(int intervals, int totalLength, float scale, float x, float y) : Intervals(intervals), TotalLength(totalLength), 
		Scale(scale), X(x), Y(y)
	{
		this->Verts = VertexArray::Create();
		this->Verts->m_DisplayType = DisplayType::LINES;

		int NumVertices = 12 + 4 * (intervals - 2);
		float* Vertices = new float[NumVertices];

		//Line: *-----------------------------*
		Vertices[0] = x;
		Vertices[1] = y;    //0
		Vertices[2] = x + 2 * scale;
		Vertices[3] = y;    //1

		/*    *
			  |-------------------
		      * */
		Vertices[4] = x;
		Vertices[5] = y - EndPointSize * scale / 2;    //3
		Vertices[6] = x;
		Vertices[7] = y + EndPointSize * scale / 2;    //4

		/*				    *
		|-------------------|
							* */
		Vertices[8] = x + 2 * scale;
		Vertices[9] = y - EndPointSize * scale / 2;    //5
		Vertices[10] = x + 2 * scale;
		Vertices[11] = y + EndPointSize * scale / 2;    //6

		for (int i = 0; i < intervals - 2; ++i)
		{
			Vertices[12 + i * 4] = x + ((float)(i + 1) /(intervals - 1)) * 2 * scale;
			Vertices[13 + i * 4] = y - IntervalSize * scale / 2;
			Vertices[14 + i * 4] = x + ((float)(i + 1) / (intervals - 1)) * 2 * scale;
			Vertices[15 + i * 4] = y + IntervalSize * scale / 2;
			CDB_EditorInfo(y + IntervalSize * scale / 2);
		}

		VertexBuffer* buffer = VertexBuffer::Create(sizeof(float) * NumVertices, Vertices);

		{
			BufferLayout layout = {
				{ShaderDataType::Float2, "pos"},
			};

			buffer->SetLayout(layout);
		}

		delete[] Vertices;

		int NumIndices = NumVertices / 2;
		uint32_t* Indices = new uint32_t[NumIndices];
		for (int i = 0; i < NumIndices; ++i)
		{
			Indices[i] = i;
		}

		IndexBuffer* indexBuffer = IndexBuffer::Create(NumIndices, Indices);
		delete[] Indices;
		this->Verts->AddVertexBuffer(buffer);
		this->Verts->SetIndexBuffer(indexBuffer);
	}

	void NumberLine::Draw(bool BindShader)
	{
		if (BindShader)
		{
			InitLineShader();
			LineShader->Bind();
		}
		CDB::Renderer::Submit(this->Verts.raw());
	}
}