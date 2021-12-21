#pragma once
#include "RendererAPI.h"
#include <glm/glm.hpp>

namespace CDB
{
	class CDBAPI RenderCommand
	{
	public:
		inline static void SetClearColour(const glm::vec4& Colour)
		{
			s_RendererAPI->SetClearColour(Colour);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const VertexArray* vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};
}