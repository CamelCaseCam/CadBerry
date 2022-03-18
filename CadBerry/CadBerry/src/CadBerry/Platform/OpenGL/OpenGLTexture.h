#pragma once
#include "CadBerry/Rendering/Texture.h"

namespace CDB
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture(const std::string& PathToImg, bool GenerateMipmap, bool FlipVertical = true);
		virtual ~OpenGLTexture() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;
	private:
		uint32_t TextureID;
		int Width, Height, NumChannels;
	};
}