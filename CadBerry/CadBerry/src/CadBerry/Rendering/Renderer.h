#pragma once
#include "CadBerry/Core.h"

#include "RendererAPI.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "RenderTarget.h"

namespace CDB
{
	class CDBAPI Renderer
	{
	public:
		static void BeginScene(RenderTarget* Target = nullptr);
		static void EndScene();

		static void Submit(const VertexArray* vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		static RenderTarget* CurrentTarget;
	};
}