#pragma once

#include "Core/Math/Math.h"
#include "Core/Types/PrimitiveTypes.h"

namespace Cookie {

	struct CameraComponent {
		Float3 m_Position;
		f32 m_Rotation;
	};

	struct CameraComponentSingl {
		// Entity that represents the current camera
		EntityID m_MainCam;
	};

} // namespace Cookie
