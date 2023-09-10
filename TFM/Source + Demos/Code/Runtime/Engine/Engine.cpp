#include "Engine.h"

#include "CookieKat/Systems/EngineSystem/EngineSystemUpdateContext.h"

#include "CookieKat/Core/Profilling/Profilling.h"

namespace CKE {
	void Engine::InitializeCore() {
		m_EngineTime.Initialize();

		m_TaskSystem.Initialize();
		m_SystemsRegistry.RegisterSystem(&m_TaskSystem);

		m_ResourceSystem.Initialize(&m_TaskSystem);
		m_SystemsRegistry.RegisterSystem(&m_ResourceSystem);
	}

	void Engine::InitializeEngine() {
		// Setup resource loaders
		//-----------------------------------------------------------------------------

		m_TextureLoader.Initialize(&m_RenderingSystem.GetRenderDevice());
		m_PipelineLoader.Initialize(&m_RenderingSystem.GetRenderDevice());
		m_MeshLoader.Initialize(&m_RenderingSystem.GetRenderDevice());
		m_CubeMapLoader.Initialize(&m_RenderingSystem.GetRenderDevice());

		m_ResourceSystem.RegisterLoader(&m_TextureLoader);
		m_ResourceSystem.RegisterLoader(&m_PipelineLoader);
		m_ResourceSystem.RegisterLoader(&m_MeshLoader);
		m_ResourceSystem.RegisterLoader(&m_MaterialLoader);
		m_ResourceSystem.RegisterLoader(&m_CubeMapLoader);

		// Rendering
		//-----------------------------------------------------------------------------

		// Hack
		m_SystemsRegistry.RegisterSystem(&m_EntitySystem);

		m_RenderingSystem.Initialize(&m_SystemsRegistry);
		m_SystemsRegistry.RegisterSystem(&m_RenderingSystem);

		m_InputSystem.Initialize();
		m_SystemsRegistry.RegisterSystem(&m_InputSystem);

		// Entities
		//-----------------------------------------------------------------------------

		m_EntitySystem.Initialize(m_SystemsRegistry);

		// Systems Registry setup
		//-----------------------------------------------------------------------------
	}

	void Engine::Update() {
		CKE_PROFILE_FRAME("MainThread");

		EngineSystemUpdateContext updateCtx;
		updateCtx.m_pSystemsRegistry = &m_SystemsRegistry;
		updateCtx.m_pEngineTime = &m_EngineTime;

		// Tick Time
		m_EngineTime.Update();

		// Update Entity World
		m_EntitySystem.Update(updateCtx);

		// Render Everything
		m_RenderingSystem.RenderFrame();

		// Cleanup necessary input state
		m_InputSystem.EndOfFrameUpdate();
	}

	void Engine::Shutdown() {
		m_EntitySystem.Shutdown();
		m_RenderingSystem.Shutdown();

		m_TaskSystem.Shutdown();
	}
} // namespace CKE
