#pragma once
#include <cdbpch.h>
#include "CadBerry/Core.h"

namespace CDB
{
	class CDBAPI Texture
	{
	public:
		virtual ~Texture() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Texture* Create(const std::string& PathToImg, bool GenerateMipmap = true, bool FlipVertical = true);
	};
}