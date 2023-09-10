#pragma once

#include "Core/Types/Function.h"
#include "Core/Types/PrimitiveTypes.h"
#include "Core/Types/String.h"

#include "Core/Time/Time.h"

#include "Entities/EntityAdmin.h"

#include "Core/Window.h"

namespace Cookie {

	struct AppData {
		WindowData m_Window;
	};

	struct GameInitData {
		WindowDescription m_WindowDesc;
		TFunction<void(EntityAdmin *const EntitiesAdmin)> m_CreateWorldFunc;
		TFunction<void(EntityAdmin *const EntitiesAdmin)> m_RegisterECSFunc;
		TFunction<void()> m_LoadResourcesFunc;
	};

	extern AppData g_AppData;
	extern EntityAdmin *g_Admin;

	namespace Application {

		void Run(GameInitData *gameInitData);

	} // namespace Application

} // namespace Cookie