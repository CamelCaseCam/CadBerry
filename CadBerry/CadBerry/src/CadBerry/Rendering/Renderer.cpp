#include <cdbpch.h>
#include "Renderer.h"
#include "RenderCommand.h"

namespace CDB
{
	RenderTarget* Renderer::CurrentTarget = nullptr;

	void Renderer::BeginScene(RenderTarget* Target)
	{
		if (Target != nullptr)
		{
			CurrentTarget = Target;
			Target->Bind();
		}
	}
	void Renderer::EndScene()
	{
		if (CurrentTarget != nullptr)
		{
			CurrentTarget->Unbind();
			CurrentTarget = nullptr;
		}
	}

	void Renderer::Submit(const VertexArray* vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}