#include <cdbpch.h>
#include "Renderer.h"
#include "RenderCommand.h"

namespace CDB
{
	RenderTarget* Renderer::CurrentTarget = nullptr;
	glm::mat4 Renderer::ProjectionViewMatrix = glm::mat4(1.0);

	void Renderer::BeginScene(OrthographicCamera& Camera, RenderTarget* Target)
	{
		if (Target != nullptr)
		{
			CurrentTarget = Target;
			Target->Bind();
		}
		ProjectionViewMatrix = Camera.GetPVMatrix();
	}

	void Renderer::BeginScene(RenderTarget* Target)
	{
		if (Target != nullptr)
		{
			CurrentTarget = Target;
			Target->Bind();
		}
		ProjectionViewMatrix = glm::mat4(1.0);
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

	void Renderer::Submit(const VertexArray* vertexArray, const Shader* shader, const glm::mat4& transform = glm::mat4(1.0f))
	{
		vertexArray->Bind();
		shader->Bind();
		shader->UploadUniformMat4("u_PVMatrix", ProjectionViewMatrix);
		shader->UploadUniformMat4("u_transform", transform);

		RenderCommand::DrawIndexed(vertexArray);
	}
}