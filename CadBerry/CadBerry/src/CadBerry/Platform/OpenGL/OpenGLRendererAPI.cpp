#include <cdbpch.h>
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace CDB
{
	void OpenGLRendererAPI::SetClearColour(const glm::vec4& Colour)
	{
		glClearColor(Colour.r, Colour.g, Colour.b, Colour.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const VertexArray* vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->BorrowIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}