#pragma once

#include "Core/Window.h"

namespace Cookie::Platform {

	void Initialize(WindowDescription *wd);

	// Returns true if the app should still be running
	bool IsRunning(void *windowHandle);

	void Shutdown();

} // namespace Cookie::Platform