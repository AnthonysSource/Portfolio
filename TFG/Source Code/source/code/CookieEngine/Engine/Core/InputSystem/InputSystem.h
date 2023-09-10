#pragma once

#include "Core/Defines.h"
#include "Core/Types/PrimitiveTypes.h"
#include "Core/Platform/Platform.h"

#include "Entities/EntityAdmin.h"

namespace Cookie {

	class InputSystem : public System {
	public:
		void Init(WindowData *window);

		void InitSignature() override;
		void Update(f32) override;
		void Shutdown();
	};

} // namespace Cookie