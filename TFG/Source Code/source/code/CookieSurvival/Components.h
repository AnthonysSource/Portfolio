#pragma once

#include "CookieEngine.h"

namespace Cookie {

	struct PlayerCharacterComponent {
		f32 m_Speed;
	};

	struct EnemyComponent {
		f32 m_Acceleration;
		f32 m_TopSpeed;
		Float3 m_Velocity;
	};

	struct SinglMainPlayerComponent {
		Float3 m_Position;
	};

	struct AttackComponent {

		// 0 = Horizontal
		// 1 = Vertical
		// 2 = Ring
		u32 m_LastAttackID;

		// Ring Attack
		f32 m_MinArea;
		f32 m_MaxArea;

		// Rectangle Attack
		f32 m_RectX;
		f32 m_RectY;

		f32 m_CooldownElapsed;
		f32 m_CooldownTotal;
	};

	struct ScoreSinglComponent {
		u32 m_CurrentScore;
	};

} // namespace Cookie