#pragma once
#include <cdbpch.h>
#include "CadBerry/Rendering/RenderTarget.h"

#include <glad/glad.h>

namespace CDB
{
	class CDBAPI OpenGLRenderTarget : public RenderTarget
	{
	public:
		OpenGLRenderTarget();
		virtual ~OpenGLRenderTarget() override;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Draw(ImVec2 Location, ImVec2 Scale) override;
		virtual void SetAsBackground(ImVec2 Location, ImVec2 Scale) override;

	private:
		GLuint TextureID;
		ImVec2 WindowSize;
		GLuint Framebuffer;
	};
}