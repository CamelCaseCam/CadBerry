#include <cdbpch.h>
#include "Shader.h"
#include "Renderer.h"
#include "CadBerry/Log.h"

#include "CadBerry/Platform/OpenGL/OpenGLShader.h"

namespace CDB
{
	Shader* Shader::Create(const std::string& VertSrc, const std::string& FragSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLShader(VertSrc, FragSrc);
		case RendererAPI::API::None:
			CDB_EditorAssert(false, "Cannot create shader with no renderer API");
			return nullptr;
		}

		CDB_BuildError("Unknown renderer API given");
		return nullptr;
	}
}