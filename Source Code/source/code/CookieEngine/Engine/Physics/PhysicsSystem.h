#pragma once

#include "Entities/EntityAdmin.h"

namespace Cookie {

	class PhysicsSystem : public System {
	public:
		void InitSignature() override;
		void Update(f32) override;
		void Shutdown();

	private:
		EntitiesView *m_View;
	};

} // namespace Cookie