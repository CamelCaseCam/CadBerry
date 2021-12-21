#pragma once
#include "CadBerry/Rendering/VertexArray.h"
#include "CadBerry/Utils/memory.h"

namespace CDB
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) override;
		virtual void SetIndexBuffer(IndexBuffer* indexBuffer) override;

		virtual const std::vector<scoped_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const IndexBuffer* BorrowIndexBuffer() const override { return m_IndexBuffer.raw(); }
	private:
		uint32_t ArrayID;

		std::vector<scoped_ptr<VertexBuffer>> m_VertexBuffers;
		scoped_ptr<IndexBuffer> m_IndexBuffer;
	};
}