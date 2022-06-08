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

	const std::unordered_map<DisplayType, int> DisplayType2GLType = { {DisplayType::TRIANGLES, GL_TRIANGLES}, {DisplayType::LINES, GL_LINES} };

	void OpenGLRendererAPI::DrawIndexed(const VertexArray* vertexArray)
	{
		vertexArray->BorrowIndexBuffer()->Bind();
		glDrawElements(DisplayType2GLType.at(vertexArray->m_DisplayType), vertexArray->BorrowIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		vertexArray->BorrowIndexBuffer()->Unbind();
	}
}