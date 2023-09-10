#pragma once

#include "Core/Types/PrimitiveTypes.h"

#include "Common.h"

namespace Cookie {

	class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void DestroyEntity(EntityID entity) = 0;
	};

	template <typename T> class ComponentArray : public IComponentArray {
	public:
		void Insert(EntityID entity, T component) {
			u64 newIndex = m_Count;
			m_EntityToIndex[entity] = newIndex;
			m_IndexToEntity[newIndex] = entity;
			m_Array[newIndex] = component;
			++m_Count;
		}

		void Remove(EntityID entity) {
			u64 indexOfRemovedEntity = m_EntityToIndex[entity];
			u64 indexOfLastElement = m_Count - 1;
			m_Array[indexOfRemovedEntity] = m_Array[indexOfLastElement];

			EntityID entityOfLastElement = m_IndexToEntity[indexOfLastElement];
			m_EntityToIndex[entityOfLastElement] = indexOfRemovedEntity;
			m_IndexToEntity[indexOfRemovedEntity] = entityOfLastElement;

			m_EntityToIndex[entity] = InvalidEntityID;
			m_IndexToEntity[indexOfLastElement] = InvalidEntityID;

			--m_Count;
		}

		T *Get(EntityID entity) { return &m_Array[m_EntityToIndex[entity]]; }

		T *At(size_t index) { return &m_Array[index]; }

		u64 Count() { return m_Count; }

		void DestroyEntity(EntityID entity) override { m_EntityToIndex[entity] = -1; }

	private:
		T m_Array[MAX_ENTITIES];
		u64 m_Count;

		u64 m_EntityToIndex[MAX_ENTITIES];
		u64 m_IndexToEntity[MAX_ENTITIES];
	};

} // namespace Cookie