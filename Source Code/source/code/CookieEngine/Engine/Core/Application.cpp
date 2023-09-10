#include "Application.h"

#include "Core/Window.h"
#include "Core/Logging/Log.h"
#include "Core/Platform/Platform.h"
#include "Core/Types/Function.h"
#include "Core/Types/PrimitiveTypes.h"
#include "Core/Profiling/Profiling.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/InputSystem/InputSystem.h"

#include "Resources/Resources.h"

#include "Render/RenderingSystem.h"
#include "Physics/PhysicsSystem.h"

#include "Entities/EntityAdmin.h"


namespace Cookie {

	AppData g_AppData;
	EntityAdmin *g_Admin = new EntityAdmin();
	RenderingSystem *g_RenderingSystem = new RenderingSystem();
	InputSystem *g_InputSystem = new InputSystem();

	namespace Application {

		void Run(GameInitData *gameInitData) {

			Log::Initialize();

			CKE_LOG_INFO(Log::Channel::Core, "Starting up Cookie Engine");
			Platform::Initialize(&gameInitData->m_WindowDesc);

			CKE_LOG_INFO(Log::Channel::Core, "Initializing Input System");
			g_InputSystem->Init(&g_AppData.m_Window);

			CKE_LOG_INFO(Log::Channel::Core, "Initializing Rendering System");
			g_RenderingSystem->Init();

			CKE_LOG_INFO(Log::Channel::Core, "Initializing Resources System");
			ResourcesSystem::Init();

			CKE_LOG_INFO(Log::Channel::Core, "Initializing Entity Admin");
			g_Admin->Init();
			g_Admin->RegisterSystem(g_InputSystem);
			g_Admin->RegisterSystem<PhysicsSystem>();
			gameInitData->m_RegisterECSFunc(g_Admin);
			g_Admin->RegisterSystem(g_RenderingSystem);

			CKE_LOG_INFO(Log::Channel::Core, "Loading Game Resources");
			gameInitData->m_LoadResourcesFunc();

			CKE_LOG_INFO(Log::Channel::Core, "Creating Game World");
			gameInitData->m_CreateWorldFunc(g_Admin);
			g_Admin->InitViews();

			CKE_LOG_INFO(Log::Channel::Core, "Starting engine loop");
			g_EngineClock.Init();

			while (Platform::IsRunning(g_AppData.m_Window.m_Handle)) {
				CKE_PROFILE_START_FRAME("MainThread");

				// Tick the engine clock
				g_EngineClock.Update();

				g_Admin->Update(g_EngineClock.m_TimeData.m_DeltaTime);
			}

			// Shutdown
			CKE_LOG_INFO(Log::Channel::Core, "Shutting down");
			g_RenderingSystem->Shutdown();
			Platform::Shutdown();
			ResourcesSystem::Shutdown();
			g_InputSystem->Shutdown();
			Log::Shutdown();

			delete g_Admin;
			delete g_RenderingSystem;
			delete g_InputSystem;
		}

	} // namespace Application
} // namespace Cookie