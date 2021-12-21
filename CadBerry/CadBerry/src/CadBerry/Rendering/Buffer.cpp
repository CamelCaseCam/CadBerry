#include <cdbpch.h>
#include "Buffer.h"
#include "CadBerry/Platform/OpenGL/OpenGLBuffer.h"
#include "Renderer.h"
#include "CadBerry/Core.h"
#include "CadBerry/Log.h"

namespace CDB
{
	VertexBuffer* VertexBuffer::Create(uint32_t size, float* vertices)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexBuffer(size, vertices);
		case RendererAPI::API::None:
			CDB_EditorAssert(false, "Cannot create vertex buffer with no renderer API");
			return nullptr;
		}

		CDB_BuildError("Unknown renderer API given");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t size, uint32_t* indices)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLIndexBuffer(size, indices);
		case RendererAPI::API::None:
			CDB_EditorAssert(false, "Cannot create index buffer with no renderer API");
			return nullptr;
		}

		CDB_BuildError("Unknown renderer API given");
		return nullptr;
	}
}