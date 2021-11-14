#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

namespace CDB
{
	class CDBAPI Shader
	{
	public:
		Shader(const std::string& VertSrc, const std::string& FragSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;
	private:
		uint32_t RendererID;
	};
}