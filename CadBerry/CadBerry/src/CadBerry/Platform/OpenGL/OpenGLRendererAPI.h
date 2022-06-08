#pragma once
#include "CadBerry/Rendering/RendererAPI.h"
#include "CadBerry/Rendering/VertexArray.h"

#include <glm/glm.hpp>

namespace CDB
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void SetClearColour(const glm::vec4& Colour) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const VertexArray* vertexArray) override;
	};
}