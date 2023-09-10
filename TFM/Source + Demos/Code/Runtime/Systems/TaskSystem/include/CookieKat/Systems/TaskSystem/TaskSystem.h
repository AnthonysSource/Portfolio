#pragma once

#include "CookieKat/Systems/EngineSystem/IEngineSystem.h"
#include "TaskScheduler.h"

namespace CKE
{
	using ITaskSet = enki::ITaskSet;
}

namespace CKE
{
	class TaskSystem : public IEngineSystem
	{
	public:

		// Lifetime
		//-----------------------------------------------------------------------------

		void Initialize();
		void Shutdown();

		// Tasks
		//-----------------------------------------------------------------------------

		inline void ScheduleTask(ITaskSet* taskSet) { m_TaskScheduler.AddTaskSetToPipe(taskSet); }
		inline void WaitForTask(ITaskSet* taskSet) { m_TaskScheduler.WaitforTask(taskSet); }

	private:

		enki::TaskScheduler m_TaskScheduler;
	};
}
