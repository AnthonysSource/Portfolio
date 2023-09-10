#pragma once
#include <imgui.h>

namespace Cookie {
	namespace ImGuiRenderer {

		void Init();
		void NewFrame();
		void Render();
		void Shutdown();

	} // namespace ImGuiRenderer
} // namespace Cookie