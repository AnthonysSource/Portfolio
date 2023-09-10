#pragma once

#include "CookieKat/Core/Serialization/Archive.h"
#include "CookieKat/Systems/Resources/ResourceTypeID.h"

namespace CKE {
	struct ResourceID
	{
		CKE_SERIALIZE(m_Value);
		u64 m_Value = 0;

		inline bool IsNotNull() const { return m_Value != 0; }
		inline u64  GetU64() const { return m_Value; }
		bool        operator==(const ResourceID& other) const { return (m_Value == other.m_Value); }
	};

	template <typename T>
	struct TResourceID : public ResourceID
	{
		CKE_SERIALIZE(m_Value);
	};
}

// Hash Implementations
//-----------------------------------------------------------------------------

namespace std {
	template <>
	struct hash<CKE::ResourceID>
	{
		std::size_t operator()(const CKE::ResourceID& k) const {
			using std::hash;
			return hash<CKE::u64>()(k.m_Value);
		}
	};
}
