#include "cdbpch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "CadBerry/Log.h"

#include "CadBerry/Platform/OpenGL/OpenGLVertexArray.h"

namespace CDB
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexArray();
		case RendererAPI::API::None:
			CDB_EditorAssert(false, "Cannot create vertex buffer with no renderer API");
			return nullptr;
		}

		CDB_BuildError("Unknown renderer API given");
		return nullptr;
	}
}