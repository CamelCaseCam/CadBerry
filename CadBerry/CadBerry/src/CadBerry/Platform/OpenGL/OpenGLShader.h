#pragma once
#include <cdbpch.h>
#include "CadBerry/Rendering/Shader.h"

namespace CDB
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& VertSrc, const std::string& FragSrc);
		virtual ~OpenGLShader() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t RendererID;
	};
}