#pragma once

#include "CadBerry/Core.h"
#include "CadBerry/Math.h"
#include "KeyCode.h"
#include "MouseCode.h"

namespace CDB
{
	enum class Keyboard;

	class CDBAPI Input
	{
	public:
		inline static bool IsKeyPressed(int KeyCode) { return s_Instance->IsKeyPressedImpl(KeyCode); }
		inline static bool IsKeyPressed(Keyboard KeyCode) { return s_Instance->IsKeyPressedImpl((int)KeyCode); }
		inline static bool IsMouseButtonPressed(int MouseButton) { return s_Instance->IsMouseButtonPressedImpl(MouseButton); }
		inline static Vec2f GetMousePos() { return s_Instance->GetMousePosImpl(); }
		inline static void SetInstance(Input* i) { s_Instance = i; }
	protected:
		virtual bool IsKeyPressedImpl(int KeyCode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual Vec2f GetMousePosImpl() = 0;
	private:
		static Input* s_Instance;
	};

	enum class Keyboard
	{
		SPACE = CDB_KEY_SPACE,
		APOSTROPHE = CDB_KEY_APOSTROPHE,
		COMMA = CDB_KEY_COMMA,
		MINUS = CDB_KEY_COMMA,
		PERIOD = CDB_KEY_PERIOD,
		SLASH = CDB_KEY_SLASH,
		ZERO = CDB_KEY_0,
		ONE = CDB_KEY_1,
		TWO = CDB_KEY_2,
		THREE = CDB_KEY_3,
		FOUR = CDB_KEY_4,
		FIVE = CDB_KEY_5,
		SIX = CDB_KEY_6,
		SEVEN = CDB_KEY_7,
		EIGHT = CDB_KEY_8,
		NINE = CDB_KEY_9,
		SEMICOLON = CDB_KEY_SEMICOLON,
		EQUAL = CDB_KEY_EQUAL,
		A = CDB_KEY_A,
		B = CDB_KEY_B,
		C = CDB_KEY_C,
		D = CDB_KEY_D,
		E = CDB_KEY_E,
		F = CDB_KEY_F,
		G = CDB_KEY_G,
		H = CDB_KEY_H,
		I = CDB_KEY_I,
		J = CDB_KEY_J,
		K = CDB_KEY_K,
		L = CDB_KEY_L,
		M = CDB_KEY_M,
		N = CDB_KEY_N,
		O = CDB_KEY_O,
		P = CDB_KEY_P,
		Q = CDB_KEY_Q,
		R = CDB_KEY_R,
		S = CDB_KEY_S,
		T = CDB_KEY_T,
		U = CDB_KEY_U,
		V = CDB_KEY_V,
		W = CDB_KEY_W,
		X = CDB_KEY_X,
		Y = CDB_KEY_Y,
		Z = CDB_KEY_Z,
		LEFT_BRACKET = CDB_KEY_LEFT_BRACKET,
		BACKSLASH = CDB_KEY_BACKSLASH,
		RIGHT_BRACKET = CDB_KEY_RIGHT_BRACKET,
		GRAVE_ACCENT = CDB_KEY_GRAVE_ACCENT,
		WORLD_1 = CDB_KEY_WORLD_1,
		WORLD_2 = CDB_KEY_WORLD_2,

		ESCAPE = CDB_KEY_ESCAPE,
		ENTER = CDB_KEY_ENTER,
		TAB = CDB_KEY_TAB,
		BACKSPACE = CDB_KEY_BACKSPACE,
		INSERT = CDB_KEY_INSERT,
		DEL = CDB_KEY_DELETE,
		RIGHT = CDB_KEY_RIGHT,
		LEFT = CDB_KEY_LEFT,
		DOWN = CDB_KEY_DOWN,
		UP = CDB_KEY_UP,
		PAGE_UP = CDB_KEY_PAGE_UP,
		PAGE_DOWN = CDB_KEY_PAGE_DOWN,
		HOME = CDB_KEY_HOME,
		END = CDB_KEY_END,
		CAPS_LOCK = CDB_KEY_CAPS_LOCK,
		SCROLL_LOCK = CDB_KEY_SCROLL_LOCK,
		NUM_LOCK = CDB_KEY_NUM_LOCK,
		PRINT_SCREEN = CDB_KEY_PRINT_SCREEN,
		PAUSE = CDB_KEY_PAUSE,
		F1 = CDB_KEY_F1,
		F2 = CDB_KEY_F2,
		F3 = CDB_KEY_F3,
		F4 = CDB_KEY_F4,
		F5 = CDB_KEY_F5,
		F6 = CDB_KEY_F6,
		F7 = CDB_KEY_F7,
		F8 = CDB_KEY_F8,
		F9 = CDB_KEY_F9,
		F10 = CDB_KEY_F10,
		F11 = CDB_KEY_F11,
		F12 = CDB_KEY_F12,
		F13 = CDB_KEY_F13,
		F14 = CDB_KEY_F14,
		F15 = CDB_KEY_F15,
		F16 = CDB_KEY_F16,
		F17 = CDB_KEY_F17,
		F18 = CDB_KEY_F18,
		F19 = CDB_KEY_F19,
		F20 = CDB_KEY_F20,
		F21 = CDB_KEY_F21,
		F22 = CDB_KEY_F22,
		F23 = CDB_KEY_F23,
		F24 = CDB_KEY_F24,
		F25 = CDB_KEY_F25,
		KP_0 = CDB_KEY_KP_0,
		KP_1 = CDB_KEY_KP_1,
		KP_2 = CDB_KEY_KP_2,
		KP_3 = CDB_KEY_KP_3,
		KP_4 = CDB_KEY_KP_4,
		KP_5 = CDB_KEY_KP_5,
		KP_6 = CDB_KEY_KP_6,
		KP_7 = CDB_KEY_KP_7,
		KP_8 = CDB_KEY_KP_8,
		KP_9 = CDB_KEY_KP_9,
		KP_DECIMAL = CDB_KEY_KP_DECIMAL,
		KP_DIVIDE = CDB_KEY_KP_DIVIDE,
		KP_MULTIPLY = CDB_KEY_KP_MULTIPLY,
		KP_SUBTRACT = CDB_KEY_KP_SUBTRACT,
		KP_ADD = CDB_KEY_KP_ADD,
		KP_ENTER = CDB_KEY_KP_ENTER,
		KP_EQUAL = CDB_KEY_KP_EQUAL,
		LEFT_SHIFT = CDB_KEY_LEFT_SHIFT,
		LEFT_CONTROL = CDB_KEY_LEFT_CONTROL,
		LEFT_ALT = CDB_KEY_LEFT_ALT,
		LEFT_SUPER = CDB_KEY_LEFT_SUPER,
		RIGHT_SHIFT = CDB_KEY_RIGHT_SHIFT,
		RIGHT_CONTROL = CDB_KEY_RIGHT_CONTROL,
		RIGHT_ALT = CDB_KEY_RIGHT_ALT,
		RIGHT_SUPER = CDB_KEY_RIGHT_SUPER,
		MENU = CDB_KEY_MENU,
	};

	enum class Mouse
	{
		BUTTON_1 = CDB_MOUSE_BUTTON_1,
		BUTTON_2 = CDB_MOUSE_BUTTON_2,
		BUTTON_3 = CDB_MOUSE_BUTTON_3,
		BUTTON_4 = CDB_MOUSE_BUTTON_4,
		BUTTON_5 = CDB_MOUSE_BUTTON_5,
		BUTTON_6 = CDB_MOUSE_BUTTON_6,
		BUTTON_7 = CDB_MOUSE_BUTTON_7,
		BUTTON_8 = CDB_MOUSE_BUTTON_8,
		LAST = CDB_MOUSE_BUTTON_LAST,
		LEFT = CDB_MOUSE_BUTTON_LEFT,
		RIGHT = CDB_MOUSE_BUTTON_RIGHT,
		MIDDLE = CDB_MOUSE_BUTTON_MIDDLE,
	};
}