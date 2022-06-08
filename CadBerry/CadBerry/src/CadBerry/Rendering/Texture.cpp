#include <cdbpch.h>
#include "Texture.h"
#include "Renderer.h"

#include "CadBerry/Platform/OpenGL/OpenGLTexture.h"

namespace CDB
{
	Texture* Texture::Create(const std::string& PathToImg, bool GenerateMipmap, bool FlipVertical)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLTexture(PathToImg, GenerateMipmap, FlipVertical);
		case RendererAPI::API::None:
			CDB_BuildFatal("Cannot create texture with no renderer API");
			return nullptr;
		}

		CDB_BuildFatal("Unknown renderer API given. You should probably create an issue on the GitHub");
		return nullptr;
	}
}