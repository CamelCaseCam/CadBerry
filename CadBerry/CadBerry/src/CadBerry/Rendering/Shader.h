#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

#include <glm/glm.hpp>

namespace CDB
{
	class CDBAPI Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const = 0;

		static Shader* Create(const std::string& VertSrc, const std::string& FragSrc);
	};
}