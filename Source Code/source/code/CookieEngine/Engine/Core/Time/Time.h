#pragma once

#include "Core/Types/PrimitiveTypes.h"

namespace Cookie {

	//-------------------------------------------------------------------------

	namespace PlatformClock {

		u64 GetTimeInNanoseconds();
	};

	//-------------------------------------------------------------------------

	struct TimeData {
		f64 m_SecondsUpTime;
		f64 m_SecondsUpTimeLastUpdate;

		f32 m_DeltaTime;

		u64 m_FrameNumber;
	};

	struct EngineClock {

		TimeData m_TimeData;

		void Init();
		void Update();
	};

	//-------------------------------------------------------------------------

	extern EngineClock g_EngineClock;

} // namespace Cookie