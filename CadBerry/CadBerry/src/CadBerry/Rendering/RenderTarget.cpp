#include <cdbpch.h>
#include "RenderTarget.h"
#include "CadBerry/Platform/OpenGL/OpenGLRenderTarget.h"

namespace CDB
{
	RenderTarget* RenderTarget::Create()
	{
		return new OpenGLRenderTarget();
	}
}