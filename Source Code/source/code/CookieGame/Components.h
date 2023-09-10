#pragma once

#include "CookieEngine.h"

namespace Cookie {

	struct RotatingComponent {
		f32 m_Speed = 5.0f;
	};

	struct FloatComponent {
		f32 m_Speed = 2.0f;
		f32 m_Amplitude = 2.0f;
	};

	struct PlayerCharacterComponent {
		f32 m_Speed = 1.0f;
	};

} // namespace Cookie