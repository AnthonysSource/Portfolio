#pragma once

#include "Core/Types/PrimitiveTypes.h"
#include <box2d/box2d.h>

namespace Cookie {

	struct PhysicsComponent {
		b2Body *m_Body;
	};

	struct PhysicsWorldSinglComponent {
		b2World *m_World;
	};

} // namespace Cookie
