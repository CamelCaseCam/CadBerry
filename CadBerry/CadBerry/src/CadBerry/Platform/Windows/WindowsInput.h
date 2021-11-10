#pragma once

#include "CadBerry/Input.h"

namespace CDB
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int KeyCode) override;
		virtual bool IsMouseButtonPressedImpl(int MouseButton) override;
		virtual Vec2f GetMousePosImpl() override;
	};
}