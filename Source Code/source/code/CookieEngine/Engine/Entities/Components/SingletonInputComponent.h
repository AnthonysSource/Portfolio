#pragma once

#include "Core/Types/PrimitiveTypes.h"
#include "Core/InputSystem/InputSystem.h"

namespace Cookie {

	struct InputEvent {
		i32 m_KeyCode;
		i32 m_ScanCode;
		i32 m_Action; // Pressed or Released
		i32 m_Mods;
	};

	struct KeyboardInputState {
		// TODO: replace size with actual real size
		bool m_KeyDown[255]; // Key Pressed this frame
		bool m_KeyHeld[255]; // Key being held this frame
		bool m_KeyUp[255];	 // Key Released this frame

		// A key can be down and up in the same frame,
		// that means that it has been pressed and already
		// released
	};

	struct MouseInputState {
		i32 m_XPos;
		i32 m_YPos;

		// This would not support extra
		// mouse buttons
		bool m_ButtonDown[3];
		bool m_ButtonHeld[3];
		bool m_ButtonUp[3];
	};

	struct InputComponent {

		KeyboardInputState m_Keyboard;
		MouseInputState m_Mouse;

		CKE_FORCE_INLINE bool IsKeyDown(int keyCode) { return m_Keyboard.m_KeyDown[keyCode]; }
		CKE_FORCE_INLINE bool IsKeyUp(int keyCode) { return m_Keyboard.m_KeyUp[keyCode]; }
		CKE_FORCE_INLINE bool IsKeyHeld(int keyCode) { return m_Keyboard.m_KeyHeld[keyCode]; }
	};

} // namespace Cookie
