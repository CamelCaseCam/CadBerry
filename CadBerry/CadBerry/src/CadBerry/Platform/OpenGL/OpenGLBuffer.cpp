#include <cdbpch.h>
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace CDB
{
	//___________________________________________________________________________________________________________
	// Vertex buffer
	//___________________________________________________________________________________________________________

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, float* vertices)
	{
		glCreateBuffers(1, &this->BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, this->BufferID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &this->BufferID);
	}
	
	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->BufferID);
	}
	
	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, this->BufferID);
	}

	//___________________________________________________________________________________________________________
	// Index buffer
	//___________________________________________________________________________________________________________

	
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, uint32_t* indices) : NumIndices(count)
	{
		glCreateBuffers(1, &this->BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
	}
	
	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->BufferID);
	}
	
	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}