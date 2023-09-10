#include "Archetype.h"
#include "CookieKat/Core/Platform/Asserts.h"

namespace CKE {
	EntityID Archetype::RemoveEntityRow(u64 entityRow) {
		CKE_ASSERT(m_NumEntities > 0);

		EntityID movedEntityID = m_RowIndexToEntity[m_NumEntities - 1];
		m_RowIndexToEntity[m_NumEntities - 1] = EntityID::Invalid();
		m_RowIndexToEntity[entityRow] = movedEntityID;

		for (auto&& componentArray : m_ArchTable) {
			componentArray.RemoveCompAt(entityRow);
		}

		m_NumEntities--;
		return movedEntityID;
	}

	u64 Archetype::AddEntityRow(EntityID associatedEntity) {
		m_NumEntities++;

		// Create a row in all of the component arrays
		// for the entity data
		u64 entityArchetypeRow = -1;
		for (auto&& componentArray : m_ArchTable) {
			entityArchetypeRow = componentArray.AppendComponentToEnd();
		}
		m_RowIndexToEntity[entityArchetypeRow] = associatedEntity;

		return entityArchetypeRow;
	}
}
