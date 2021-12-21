#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

namespace CDB
{
	class CDBAPI Shader
	{
	public:
		virtual ~Shader() {};

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string& VertSrc, const std::string& FragSrc);
	};
}