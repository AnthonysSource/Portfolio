#include "IMGUI_Impl.h"

#include "Core/Application.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace Cookie::ImGuiRenderer {

	void ImGuiRenderer::Init() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)g_AppData.m_Window.m_Handle, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void ImGuiRenderer::NewFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiRenderer::Render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiRenderer::Shutdown() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

} // namespace Cookie::ImGuiRenderer