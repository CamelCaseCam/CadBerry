#pragma once

#include "CadBerry.h"

namespace dmd
{
	class DeepMDViewport : public CDB::Viewport
	{
	public:
		DeepMDViewport() : CDB::Viewport("DeepMD viewport") {}
		virtual ~DeepMDViewport() override;

		virtual void GUIDraw() override;
	};
}