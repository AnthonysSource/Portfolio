#include "InputSystem.h"

#include "Core/Application.h"
#include "Core/Types/Containers.h"
#include "Core/Types/PrimitiveTypes.h"

#include "Core/Logging/Log.h"
#include "Core/Profiling/Profiling.h"

#include "Entities/Components/SingletonInputComponent.h"
#include "Core/Window.h"

#include <GLFW/glfw3.h>

namespace Cookie {

	void LogInput(InputEvent e, InputComponent inputComp);
	void WindowKeyEventHandle(GLFWwindow *window, int key, int scancode, int action, int mods);

	// -----------------------------------------------------------------

	// Contains a buffer of the input events that have been
	// registered in the current frame
	static TQueue<InputEvent> s_InputEventsBuffer;
	static InputComponent g_InputComponent;

	// -----------------------------------------------------------------

	void InputSystem::Init(WindowData *window) { glfwSetKeyCallback((GLFWwindow *)window->m_Handle, WindowKeyEventHandle); }

	void InputSystem::InitSignature() {}

	void InputSystem::Shutdown() {}

	void InputSystem::Update(f32 dt) {
		CKE_PROFILE_EVENT();

		InputComponent *inputComp = g_Admin->GetSinglComponent<InputComponent>();

		// Reset previous input state
		for (size_t i = 0; i < 255; i++) {
			// We dont reset keyheld because that gets
			// reset in a keyup event
			inputComp->m_Keyboard.m_KeyDown[i] = false;
			inputComp->m_Keyboard.m_KeyUp[i] = false;
		}

		// Poll All Window Events
		glfwPollEvents();

		// Process input events
		for (i32 i = 0; i < s_InputEventsBuffer.size(); i++) {
			InputEvent e = s_InputEventsBuffer.front();

			// Logging to inspect system state
			// LogInput(e);

			s_InputEventsBuffer.pop();
		}
	}


	// Input State Logging
	// -----------------------------------------------------------------

	void WindowKeyEventHandle(GLFWwindow *window, int key, int scancode, int action, int mods) {
		// Push input event into queue
		InputEvent e;
		e.m_KeyCode = key;
		e.m_ScanCode = scancode;
		e.m_Action = action;
		e.m_Mods = mods;
		s_InputEventsBuffer.push(e);

		InputComponent *inputComp = g_Admin->GetSinglComponent<InputComponent>();

		// Save input data
		if (action == 1) {
			inputComp->m_Keyboard.m_KeyDown[key] = true;
			inputComp->m_Keyboard.m_KeyHeld[key] = true;
		} else if (action == 0) {
			inputComp->m_Keyboard.m_KeyHeld[key] = false;
			inputComp->m_Keyboard.m_KeyUp[key] = true;
		}
	}

	void LogInput(InputEvent e, InputComponent inputComp) {
		// CKE_LOG_INFO("Event Data -> Key Name: {} / Keycode: {} / Scancode : {} / Action: {} / Mod: {}",
		//			 glfwGetKeyName(e.m_KeyCode, e.m_ScanCode), e.m_KeyCode, e.m_ScanCode, e.m_Action, e.m_Mods);

		// if (e.m_Action == 0) {
		// 	CKE_LOG_INFO("[{}] Released", glfwGetKeyName(e.m_KeyCode, e.m_ScanCode));
		// } else if (e.m_Action == 1) {
		// 	CKE_LOG_INFO("[{}] Pressed", glfwGetKeyName(e.m_KeyCode, e.m_ScanCode));
		// }

		CKE_LOG_INFO(Log::Channel::Input, "KeyName: %s / Event: %d / Down: %d / Held: %d / Up: %d",
					 glfwGetKeyName(e.m_KeyCode, e.m_ScanCode), e.m_Action, inputComp.m_Keyboard.m_KeyDown[e.m_KeyCode],
					 inputComp.m_Keyboard.m_KeyHeld[e.m_KeyCode], inputComp.m_Keyboard.m_KeyUp[e.m_KeyCode]);
	}

} // namespace Cookie
