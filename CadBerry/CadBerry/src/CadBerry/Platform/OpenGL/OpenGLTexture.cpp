#include <cdbpch.h>
#include "OpenGLTexture.h"
#include "CadBerry/Rendering/Image/stb_image.h"
#include "CadBerry/Rendering/Image/CDBImage.h"

#include "CadBerry/Log.h"

#include <glad/glad.h>

namespace CDB
{
	OpenGLTexture::OpenGLTexture(const std::string& PathToImg, bool GenerateMipmap, bool FlipVertical)
	{
		stbi_set_flip_vertically_on_load(FlipVertical);
		unsigned char* Data = stbi_load(PathToImg.c_str(), &Width, &Height, &NumChannels, 0);    //Loads the data into a temporary array
		if (Data)
		{
			glGenTextures(1, &this->TextureID);
			glBindTexture(GL_TEXTURE_2D, this->TextureID);

			if (NumChannels == 3)    //RGB image
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->Width, this->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Data);
				if (GenerateMipmap)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}
			}
			else if (NumChannels == 4)    //RGBA image
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->Width, this->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);
				if (GenerateMipmap)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}
			}
			else
			{
				CDB_EditorError("File \"{0}\" has unsupported file format for texture");
				delete this;
			}
		}
		else
		{
			CDB_EditorError("File \"{0}\" has unsupported file format for texture");
			delete this;
		}
		stbi_image_free(Data);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &this->TextureID);
	}

	void OpenGLTexture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, this->TextureID);
	}
	void OpenGLTexture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}