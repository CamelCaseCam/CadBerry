#include <cdbpch.h>
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace CDB
{
	//I originally planned to make this an unordered_map, but switch gives the compiler more freedom to optimize it
	GLenum ShaderDataType2OpenGL(ShaderDataType dtype)
	{
		switch (dtype)
		{
		case CDB::ShaderDataType::Float:     return GL_FLOAT;
		case CDB::ShaderDataType::Float2:    return GL_FLOAT;
		case CDB::ShaderDataType::Float3:    return GL_FLOAT;
		case CDB::ShaderDataType::Float4:    return GL_FLOAT;
		case CDB::ShaderDataType::Mat3:      return GL_FLOAT;
		case CDB::ShaderDataType::Mat4:      return GL_FLOAT;
		case CDB::ShaderDataType::Int:       return GL_INT;
		case CDB::ShaderDataType::Int2:      return GL_INT;
		case CDB::ShaderDataType::Int3:      return GL_INT;
		case CDB::ShaderDataType::Int4:      return GL_INT;
		case CDB::ShaderDataType::Bool:      return GL_BOOL;
		}

		CDB_EditorAssert(false, "Unknown ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &this->ArrayID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		CDB_EditorInfo(this->ArrayID);
		glDeleteVertexArrays(1, &this->ArrayID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(this->ArrayID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(VertexBuffer* vertexBuffer)
	{
		CDB_EditorAssert(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer must have a layout");
		glBindVertexArray(this->ArrayID);
		vertexBuffer->Bind();

		uint32_t Index = 0;
		for (const BufferElement& Elem : vertexBuffer->GetLayout())
		{
			glEnableVertexAttribArray(Index);
			glVertexAttribPointer(Index, Elem.GetElementCount(), ShaderDataType2OpenGL(Elem.ElementType), Elem.Normalized ? GL_TRUE : GL_FALSE, vertexBuffer->GetLayout().GetStride(), (const void*)Elem.Offset);
			++Index;
		}

		m_VertexBuffers.push_back(vertexBuffer);    //VertexArray takes ownership of the vertex buffer
	}

	void OpenGLVertexArray::SetIndexBuffer(IndexBuffer* indexBuffer)
	{
		glBindVertexArray(this->ArrayID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;    //VertexArray takes ownership of the index buffer
	}
}