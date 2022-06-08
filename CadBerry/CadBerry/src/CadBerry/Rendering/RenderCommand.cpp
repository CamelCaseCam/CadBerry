#include <cdbpch.h>
#include "RenderCommand.h"
#include "CadBerry/Platform/OpenGL/OpenGLRendererAPI.h"

namespace CDB
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}