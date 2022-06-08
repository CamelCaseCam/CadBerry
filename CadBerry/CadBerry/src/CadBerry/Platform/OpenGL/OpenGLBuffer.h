#pragma once
#include "CadBerry/Rendering/Buffer.h"

namespace CDB
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size, float* vertices);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetLayout(const BufferLayout& Layout) override { this->Layout = Layout; };
		virtual const BufferLayout& GetLayout() const override { return Layout; }

	private:
		uint32_t BufferID;
		BufferLayout Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count, uint32_t* indices);
		virtual ~OpenGLIndexBuffer() {}

		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint32_t GetCount() const override { return NumIndices; }

	private:
		uint32_t BufferID;
		uint32_t NumIndices;
	};
}