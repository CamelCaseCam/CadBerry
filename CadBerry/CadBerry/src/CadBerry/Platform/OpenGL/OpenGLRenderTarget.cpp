#include <cdbpch.h>
#include "OpenGLRenderTarget.h"
#include "CadBerry/Log.h"
#include "CadBerry/Rendering/RenderCommand.h"
#include "CadBerry/Application.h"

#include <GLFW/glfw3.h>

#include "imgui.h"

namespace CDB
{
	OpenGLRenderTarget::OpenGLRenderTarget()
	{
		glCreateFramebuffers(1, &Framebuffer);

		glGenTextures(1, &TextureID);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		
		WindowSize = ImVec2(1920, 1080);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowSize.x, WindowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, TextureID);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			CDB_EditorError("Incomplete framebuffer");
	}

	OpenGLRenderTarget::~OpenGLRenderTarget()
	{
		glDeleteFramebuffers(1, &Framebuffer);
	}

	void OpenGLRenderTarget::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, 0);
		glViewport(0, 0, WindowSize.x, WindowSize.y);
	}

	void OpenGLRenderTarget::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());
	}

	void OpenGLRenderTarget::Draw(ImVec2 Location, ImVec2 Scale)
	{
		ImGui::Image((ImTextureID)TextureID, ImVec2(Location.x * Scale.x, Location.y * Scale.y), ImVec2(0, 1), ImVec2(1, 0));
	}

	void OpenGLRenderTarget::SetAsBackground(ImVec2 Location, ImVec2 Scale)
	{
		ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)TextureID, ImVec2(Location.x * Scale.x, Location.y * Scale.y), ImVec2(0, 1), ImVec2(1, 0));
	}
}