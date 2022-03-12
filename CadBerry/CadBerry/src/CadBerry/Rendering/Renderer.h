#pragma once
#include "CadBerry/Core.h"

#include "RendererAPI.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Shader.h"

#include <glm/glm.hpp>

namespace CDB
{
	class CDBAPI Renderer
	{
	public:
		static void BeginScene(OrthographicCamera& Camera, RenderTarget* Target = nullptr);
		static void BeginScene(RenderTarget* Target = nullptr);
		static void EndScene();

		static void Submit(const VertexArray* vertexArray);
		static void Submit(const VertexArray* vertexArray, const Shader* shader, const glm::mat4& transform);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		static RenderTarget* CurrentTarget;
		static glm::mat4 ProjectionViewMatrix;
	};
}