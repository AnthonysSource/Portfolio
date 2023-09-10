#pragma once

#include "CookieKat/Core/Memory/Memory.h"
#include "CookieKat/Core/Platform/Asserts.h"

#include "IDs.h"

namespace CKE {
	// Works as a packed unordered array
	class ComponentArray
	{
	public:
		ComponentArray(ComponentTypeID componentID, u64 numMaxElements, u64 compSizeInBytes);
		~ComponentArray();
		ComponentArray(const ComponentArray& other) = delete;
		ComponentArray(ComponentArray&& other) noexcept;
		ComponentArray& operator=(const ComponentArray& other) = delete;
		ComponentArray& operator=(ComponentArray&& other) noexcept;

		//-----------------------------------------------------------------------------

		inline u64 AppendComponentToEnd();
		inline u8* GetCompAtIndex(u64 index);
		// Returns the index of the element that has been moved
		// to fill the hole left by the removed component
		inline u64 RemoveCompAt(u64 index);
		inline u64 GetCompSizeInBytes() const;

	private:
		u8*         m_pData;
		u64         m_NumElements;
		u64         m_NumMaxElements;
		u64         m_ElementSizeInBytes;
		ComponentTypeID m_ComponentID;
	};
}

// Template implementation
//-----------------------------------------------------------------------------

namespace CKE {
	inline ComponentArray::ComponentArray(ComponentTypeID componentID, u64 numMaxElements, u64 compSizeInBytes) {
		m_ComponentID = componentID;
		m_NumMaxElements = numMaxElements;
		m_NumElements = 0;
		m_ElementSizeInBytes = compSizeInBytes;
		m_pData = static_cast<u8*>(CKE::Alloc(compSizeInBytes * numMaxElements));
	}

	inline ComponentArray::~ComponentArray() {
		if (m_pData != nullptr) {
			CKE::Free(m_pData);
		}
	}

	inline ComponentArray::ComponentArray(ComponentArray&& other) noexcept {
		m_ComponentID = other.m_ComponentID;
		m_NumMaxElements = other.m_NumMaxElements;
		m_NumElements = other.m_NumElements;
		m_ElementSizeInBytes = other.m_ElementSizeInBytes;
		CKE_ASSERT(other.m_pData != nullptr);
		m_pData = other.m_pData;
		other.m_pData = nullptr;
	}

	inline ComponentArray& ComponentArray::operator=(ComponentArray&& other) noexcept {
		m_ComponentID = other.m_ComponentID;
		m_NumMaxElements = other.m_NumMaxElements;
		m_NumElements = other.m_NumElements;
		m_ElementSizeInBytes = other.m_ElementSizeInBytes;
		CKE_ASSERT(other.m_pData != nullptr);
		m_pData = other.m_pData;
		other.m_pData = nullptr;
		return *this;
	}

	u64 ComponentArray::AppendComponentToEnd() {
		CKE_ASSERT(m_NumElements < m_NumMaxElements); // Ran out of space
		u64 rowIndex = m_NumElements;
		m_NumElements++;
		return rowIndex;
	}

	u8* ComponentArray::GetCompAtIndex(u64 index) {
		CKE_ASSERT(index < m_NumElements); // Trying to access a component that doesn't exist
		return m_ElementSizeInBytes * index + m_pData;
	}

	u64 ComponentArray::RemoveCompAt(u64 index) {
		CKE_ASSERT(index < m_NumElements);
		u8* elementToRemove = GetCompAtIndex(index);
		u64 lastIndex = m_NumElements != 0 ? m_NumElements - 1 : 0;
		u8* lastElement = GetCompAtIndex(lastIndex);

		memcpy(elementToRemove, lastElement, m_ElementSizeInBytes);
		m_NumElements--;
		return lastIndex;
	}

	u64 ComponentArray::GetCompSizeInBytes() const { return m_ElementSizeInBytes; }
}
