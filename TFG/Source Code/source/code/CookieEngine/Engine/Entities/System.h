#pragma once

#include "Core/Types/PrimitiveTypes.h"
#include "Core/Types/Containers.h"

#include "Entities/EntityAdmin.h"

#include "Common.h"

namespace Cookie {

	class EntityAdmin;

	class System {
	public:
		virtual void InitSignature() = 0;
		virtual void Update(f32 dt) = 0;

	protected:
		Signature m_Signature{};
		EntityAdmin *m_Admin;

		template <typename T> void SetRequiredComponent() { m_Signature.set(m_Admin->GetComponentSignatureID<T>(), true); }

		friend class EntityAdmin;
	};

} // namespace Cookie
