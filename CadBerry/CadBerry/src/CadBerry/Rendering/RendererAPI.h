#pragma once
#include "CadBerry/Core.h"

#include "VertexArray.h"

#include <glm/glm.hpp>

namespace CDB
{
	class CDBAPI RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		virtual void SetClearColour(const glm::vec4& Colour) = 0;
		virtual void Clear() = 0;

		//Doesn't take ownership of the vertex array
		virtual void DrawIndexed(const VertexArray* vertexArray) = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}