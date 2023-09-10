#pragma once

#include "Core/Math/Math.h"
#include "Core/Types/PrimitiveTypes.h"

namespace Cookie {

	struct TransformComponent {
		Float3 m_Position;
		f32 m_Rotation;
		f32 m_Scale;
	};

} // namespace Cookie
