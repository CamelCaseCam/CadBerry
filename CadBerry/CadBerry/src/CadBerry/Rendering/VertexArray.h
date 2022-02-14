#pragma once
#include "CadBerry/Core.h"
#include "CadBerry/Rendering/Buffer.h"
#include "CadBerry/Utils/memory.h"

namespace CDB
{
	enum class DisplayType
	{
		TRIANGLES,
		LINES
	};

	class CDBAPI VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(VertexBuffer* vertexBuffer) = 0;
		virtual void SetIndexBuffer(IndexBuffer* indexBuffer) = 0;

		//Gives a reference to the pointers, but the object stays owned by this class
		virtual const std::vector<scoped_ptr<VertexBuffer>>& GetVertexBuffers() const = 0;

		//Lends the raw pointer
		virtual const IndexBuffer* BorrowIndexBuffer() const = 0; 

		static VertexArray* Create();

		DisplayType m_DisplayType = DisplayType::TRIANGLES;
	};
}