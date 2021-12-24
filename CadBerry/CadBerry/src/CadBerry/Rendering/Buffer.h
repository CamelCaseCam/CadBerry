#pragma once
#include "CadBerry/Core.h"
#include "CadBerry/Log.h"
#include <cdbpch.h>

namespace CDB
{
	enum class ShaderDataType
	{
		None, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType DataType)
	{
		switch (DataType)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		CDB_EditorAssert(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		ShaderDataType ElementType;
		uint32_t Offset;
		uint32_t Size;
		std::string Name;
		bool Normalized;

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& Name, bool normalized = false) : ElementType(type), Name(Name), 
			Offset(0), Size(ShaderDataTypeSize(type)), Normalized(normalized)
		{

		}

		uint32_t GetElementCount() const
		{
			switch (ElementType)
			{
			case ShaderDataType::Bool:    return 1;
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Mat3:    return 3 * 3;
			case ShaderDataType::Mat4:    return 4 * 4;
			}
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& Elements) : Elements(Elements) 
		{
			CalculateOffsetsAndStride();
		}

		BufferLayout() {}

		inline const std::vector<BufferElement>& GetElements() const { return Elements; }

		inline uint32_t GetStride() const { return Stride; }

		std::vector<BufferElement>::iterator begin() { return Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return Elements.end(); }
	private:
		uint32_t Stride;
		std::vector<BufferElement> Elements;

		void CalculateOffsetsAndStride()
		{
			uint32_t Offset = 0;
			Stride = 0;

			for (auto& Element : Elements)
			{
				Element.Offset = Offset;
				Offset += Element.Size;
				Stride += Element.Size;
			}
		}
	};

	class CDBAPI VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& Layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static VertexBuffer* Create(uint32_t size, float* vertices);
	};

	class CDBAPI IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t count, uint32_t* indices);
	};
}