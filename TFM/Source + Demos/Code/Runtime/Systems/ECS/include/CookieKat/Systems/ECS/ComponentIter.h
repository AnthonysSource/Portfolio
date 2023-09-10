#pragma once

#include "CookieKat/Core/Containers/Containers.h"

#include "IDs.h"
#include "Archetype.h"
#include "ComponentIter.h"

//-----------------------------------------------------------------------------

namespace CKE {
	// TODO: Still in development
	// Data structures for an advanced component querying method

	enum class QueryOp
	{
		And,
		Or,
		Not,
		Optional,
	};

	enum class QueryAccess
	{
		ReadWrite,
		ReadOnly,
		WriteOnly
	};

	struct QueryElement
	{
		ComponentTypeID m_ComponentID;
		QueryAccess m_Access = QueryAccess::ReadWrite;
		QueryOp     m_Op = QueryOp::And;
	};

	struct QueryInfo
	{
		Array<QueryElement, 16> m_QueryElements;
		u32                     m_QueryElementsCount;
	};
}

//-----------------------------------------------------------------------------

namespace CKE {
	// Forward Declarations

	class EntityDatabase;
	class Archetype;
	class ComponentArray;

	//-----------------------------------------------------------------------------

	struct ArchetypeColumnPair
	{
		Archetype*               m_pArch;
		ArchetypeComponentColumn m_Column;
	};

	// Lightweight end of all component "range for" iterators
	// We use this custom struct to avoid copying the entire iterator to check
	// if it arrived to the end
	struct ComponentIterEnd
	{
		u64 m_NumEntitiesTotal;
	};

	//-----------------------------------------------------------------------------

	// Iterator for single-component queries
	class ComponentIter
	{
	public:
		// Setup
		//-----------------------------------------------------------------------------

		ComponentIter() = default;
		ComponentIter(EntityDatabase* pEntityAdmin, ComponentTypeID componentID);

		void Initialize(EntityDatabase* pEntityAdmin, ComponentTypeID componentID);

		// Utility Accessors
		//-----------------------------------------------------------------------------

		// Returns the total number of elements/entities in the iterator
		inline u64 GetNumElements() const;

		// Range-for iterator
		//-----------------------------------------------------------------------------

		inline ComponentIter    begin();
		inline ComponentIterEnd end();
		inline bool             operator!=(const ComponentIterEnd& other) const;
		inline void             operator++();
		inline void*            operator*();

	protected:
		// Base setup function that must be called to begin the component iterator
		inline void SetupBegin();

	protected:
		u64 m_CurrRowInArch = 0;
		u64 m_CurrCompColumn = 0;

		u64 m_CurrArchAccessDataIndex = 0; // Current index in the archetype component access data
		u64 m_NumRowsInCurrArch = 0;       // Total number of components in current archetype

		u64 m_NumEntitiesTotal = 0;     // Total number of entities to iterate in all archetypes
		u64 m_NumEntitiesProcessed = 0; // Total number of entities already iterated

		Vector<ArchetypeColumnPair> m_CompArchAccessData; // Data to access a component in a given archetype

		// Cached variables to avoid constant lookups
		Archetype*      m_pCurrArch = nullptr;
		ComponentArray* m_pCurrCompArray = nullptr;
	};

	//-----------------------------------------------------------------------------

	// Interface for a tuple of components returned by an iterator
	class ComponentTuple
	{
		friend class MultiComponentIter;

	public:
		inline void* GetComponent(ComponentTypeID componentID);

		template <typename T>
		inline T* GetComponent(u64 componentIndex);

	private:
		Vector<void*> m_Components;
	};

	//-----------------------------------------------------------------------------

	// Iterator for multi-component queries
	class MultiComponentIter
	{
	public:
		// Setup
		//-----------------------------------------------------------------------------

		MultiComponentIter() = default;
		MultiComponentIter(EntityDatabase* pEntityAdmin, Vector<ComponentTypeID>& componentID);

		void Initialize(EntityDatabase* pEntityAdmin, Vector<ComponentTypeID>& componentID);

		// Utility Accessors
		//-----------------------------------------------------------------------------

		// Returns the total number of elements/entities in the iterator
		inline u64 GetNumElements() const;

		// Range-for iterator
		//-----------------------------------------------------------------------------

		inline MultiComponentIter begin();
		inline ComponentIterEnd   end();
		inline bool               operator!=(const ComponentIterEnd& other) const;
		inline void               operator++();
		inline void               operator+(int i);
		inline ComponentTuple*    operator*();

	protected:
		inline void SetArchetypeToIterate(ArchetypeID archID);
		inline void BaseBeginIteratorSetup();

	protected:
		u64         m_CurrRowInArch = 0;
		Vector<u64> m_CurrCompColumnsInArch;

		// Cached Variables to avoid constant lookups
		Vector<ComponentArray*> m_pCurrCompArrays;
		Archetype*              m_pCurrArch = nullptr;

		u64 m_NumCompsInCurrArch = 0; // Total number of components in current archetype
		u64 m_CurrArchIDIndex = 0;    // Current Archetype index in the matched arch IDS

		Vector<ComponentTypeID> m_CompsToIterate;
		Vector<ArchetypeID> m_MatchedArchIDs;

		Map<ComponentTypeID, Map<ArchetypeID, ArchetypeComponentColumn>>* m_pComponentToArchetypes = nullptr;
		Map<ArchetypeID, Archetype*>*                                 m_IDToArchetype = nullptr;

		u64 m_NumEntitiesTotal = 0;    // Total number of components to iterate in all archetypes
		u64 m_NumEntitiesIterated = 0; // Total number of components already iterated

		ComponentTuple m_OutCompTuple{}; // Tuple that the iterator returns each iter
	};
}

// Template implementations
//-----------------------------------------------------------------------------

namespace CKE {
	// Multi component iterator
	//-----------------------------------------------------------------------------

	u64 MultiComponentIter::GetNumElements() const {
		return m_NumEntitiesTotal;
	}

	void MultiComponentIter::SetArchetypeToIterate(ArchetypeID archID) {
		// Setup necesary data to iterate the next archetype
		ArchetypeID newArchID = archID;
		m_pCurrArch = m_IDToArchetype->at(newArchID);
		m_NumCompsInCurrArch = m_pCurrArch->m_NumEntities;

		m_CurrCompColumnsInArch.clear();
		for (ComponentTypeID const& compID : m_CompsToIterate) {
			u64 compCol = m_pComponentToArchetypes->at(compID).at(newArchID);
			m_CurrCompColumnsInArch.emplace_back(compCol);
			m_pCurrCompArrays.emplace_back(&m_pCurrArch->m_ArchTable[compCol]);
		}
	}

	void MultiComponentIter::BaseBeginIteratorSetup() {
		SetArchetypeToIterate(m_MatchedArchIDs[0]);
	}

	MultiComponentIter MultiComponentIter::begin() {
		BaseBeginIteratorSetup();
		return *this;
	}

	ComponentIterEnd MultiComponentIter::end() {
		ComponentIterEnd iterEnd{m_NumEntitiesTotal};
		return iterEnd;
	}

	bool MultiComponentIter::operator!=(const ComponentIterEnd& other) const {
		return m_NumEntitiesIterated != other.m_NumEntitiesTotal;
	}

	void MultiComponentIter::operator++() {
		m_CurrRowInArch++;

		// If we exhausted the current archetype, go to the next one
		if (m_CurrRowInArch >= m_NumCompsInCurrArch) {
			// Update the entities processed counter
			m_NumEntitiesIterated += m_NumCompsInCurrArch;

			// Advance to the next archetype
			m_CurrRowInArch = 0;
			m_CurrArchIDIndex++;

			// If this is true then we finished iterating all of the archetypes
			if (m_CurrArchIDIndex >= m_MatchedArchIDs.size()) {
				m_CurrArchIDIndex = 0;
			}

			// Setup necesary data to iterate the next archetype
			SetArchetypeToIterate(m_MatchedArchIDs[m_CurrArchIDIndex]);
		}
	}

	inline void MultiComponentIter::operator+(int i) {
		m_CurrRowInArch += i;
		while (m_CurrRowInArch >= m_NumCompsInCurrArch) {
			m_CurrArchIDIndex++;
			m_CurrRowInArch -= m_NumCompsInCurrArch;

			if (m_CurrArchIDIndex >= m_MatchedArchIDs.size()) {
				m_CurrArchIDIndex = 0;
			}

			m_pCurrArch = m_IDToArchetype->at(m_MatchedArchIDs[m_CurrArchIDIndex]);
			m_NumCompsInCurrArch = m_pCurrArch->m_NumEntities;
		}

		SetArchetypeToIterate(m_MatchedArchIDs[m_CurrArchIDIndex]);
	}

	ComponentTuple* MultiComponentIter::operator*() {
		auto& compTupleArr = m_OutCompTuple.m_Components;
		compTupleArr.clear();
		for (int i = 0; i < m_CompsToIterate.size(); ++i) {
			compTupleArr.push_back(m_pCurrCompArrays[i]->GetCompAtIndex(m_CurrRowInArch));
		}
		return &m_OutCompTuple;
	}

	// Component iterator implementation
	//-----------------------------------------------------------------------------

	u64 ComponentIter::GetNumElements() const {
		return m_NumEntitiesTotal;
	}

	void ComponentIter::SetupBegin() {
		if (m_NumEntitiesTotal > 0) {
			// Get access data for the first element
			u64 componentColumn = m_CompArchAccessData[0].m_Column;

			// Setup the starting point of the iterator
			m_pCurrArch = m_CompArchAccessData[0].m_pArch;
			m_CurrCompColumn = componentColumn;
			m_NumRowsInCurrArch = m_pCurrArch->m_NumEntities;
			m_pCurrCompArray = &m_pCurrArch->m_ArchTable[componentColumn];
		}
	}

	ComponentIter ComponentIter::begin() {
		SetupBegin();
		return *this;
	}

	ComponentIterEnd ComponentIter::end() {
		ComponentIterEnd iter{};
		iter.m_NumEntitiesTotal = m_NumEntitiesTotal;
		return iter;
	}

	bool ComponentIter::operator!=(const ComponentIterEnd& other) const {
		return m_NumEntitiesProcessed != other.m_NumEntitiesTotal;
	}

	void ComponentIter::operator++() {
		m_CurrRowInArch++;

		// If we exhausted the current archetype, go to the next one
		[[unlikely]]
		if (m_CurrRowInArch >= m_NumRowsInCurrArch) {
			// Update the entities processed counter
			m_NumEntitiesProcessed += m_NumRowsInCurrArch;

			// Advance to the next archetype
			m_CurrRowInArch = 0;
			m_CurrArchAccessDataIndex++;

			// If this is true then we finished iterating all of the archetypes
			if (m_CurrArchAccessDataIndex >= m_CompArchAccessData.size()) {
				m_CurrArchAccessDataIndex = 0;
			}

			// Setup necesary data to iterate the next archetype
			m_CurrCompColumn = m_CompArchAccessData[m_CurrArchAccessDataIndex].m_Column;

			// Cache the archetype and component array ptr because
			// these only change when changing archetypes
			m_pCurrArch = m_CompArchAccessData[m_CurrArchAccessDataIndex].m_pArch;
			m_pCurrCompArray = &m_pCurrArch->m_ArchTable[m_CurrCompColumn];

			// Set new iter chunk total size
			m_NumRowsInCurrArch = m_pCurrArch->m_NumEntities;
		}
	}

	void* ComponentIter::operator*() {
		return m_pCurrCompArray->GetCompAtIndex(m_CurrRowInArch);
	}

	// Component tuple implementation
	//-----------------------------------------------------------------------------

	inline void* ComponentTuple::GetComponent(u64 componentIndex) {
		return m_Components[componentIndex];
	}

	template <typename T>
	T* ComponentTuple::GetComponent(u64 componentIndex) {
		return reinterpret_cast<T*>(GetComponent(componentIndex));
	}
}
