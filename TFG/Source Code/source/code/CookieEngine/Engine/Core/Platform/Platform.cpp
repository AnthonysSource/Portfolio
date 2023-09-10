#include "Platform.h"

#include "Core/Application.h"
#include "Core/Logging/Log.h"

#include <GLFW/glfw3.h>

namespace Cookie::Platform {

	void Initialize(Cookie::WindowDescription *wd) {
		WindowData *window = &g_AppData.m_Window;

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Set refresh rate
		if (wd->m_RefreshRate != 0) {
			glfwWindowHint(GLFW_REFRESH_RATE, wd->m_RefreshRate);
		} else {
			glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
		}

		// Create window
		if (wd->m_IsFullScreen) {
			window->m_Handle = glfwCreateWindow(wd->m_Width, wd->m_Height, wd->m_Title.c_str(), glfwGetPrimaryMonitor(), NULL);
		} else {
			window->m_Handle = glfwCreateWindow(wd->m_Width, wd->m_Height, wd->m_Title.c_str(), NULL, NULL);
		}

		// If the window creation failed, exit early
		if (!window->m_Handle) {
			glfwTerminate();
			CKE_LOG_CRITICAL(Log::Channel::Core, "Engine window couldn't be created");
		}

		// Save a copy of the window settings
		window->m_Width = wd->m_Width;
		window->m_Height = wd->m_Height;
		window->m_Title = wd->m_Title;
	}

	bool IsRunning(void *windowHandle) { return !glfwWindowShouldClose((GLFWwindow *)windowHandle); }

	void Shutdown() { glfwTerminate(); }

} // namespace Cookie::Platform