#pragma once

#include "Core/Defines.h"
#include "Core/Types/Containers.h"
#include "Core/Types/PrimitiveTypes.h"
#include "Core/Math/Math.h"

#include "Entities/Common.h"
#include "Entities/ComponentArray.h"
#include "Entities/System.h"

namespace Cookie {

	// -----------------------------------------------------------------

	class System;

	struct EntitiesView {
		// Signature of the entities in this view
		Signature m_ViewSignature{};
		// Entities in the view
		TSet<EntityID> m_Entities{};
	};

	class EntityAdmin {

	public:
		void Init();
		void InitViews();
		void Update(f32 deltaTime);
		void Shutdown();

		// Entities
		// -----------------------------------------------------------------

		EntityID CreateEntity();
		void DestroyEntity(EntityID entity);

		// Singleton Components
		// -----------------------------------------------------------------

		template <typename T> void RegisterSinglComponent() {
			size_t typeID = typeid(T).hash_code();
			T *singlComp = new T();
			m_SingletonComponents.insert({typeID, (void *)singlComp});
		};

		template <typename T> T *GetSinglComponent() {
			size_t typeID = typeid(T).hash_code();
			CKE_ASSERT(m_SingletonComponents.find(typeID) != m_SingletonComponents.end(),
					   "Trying to access a singleton component that doesn't exist");
			return (T *)(m_SingletonComponents[typeID]);
		}

		// Components
		// -----------------------------------------------------------------

		template <typename T> void RegisterComponent() {
			size_t typeID = typeid(T).hash_code();
			ComponentArray<T> *array = new ComponentArray<T>();
			m_ComponentArrays.insert({typeID, array});
			m_ComponentSignatureIndex.insert({typeID, m_NextComponentIndex});
			++m_NextComponentIndex;
		};

		template <typename T> void AddComponent(EntityID entity, T component) {
			GetComponentArray<T>()->Insert(entity, component);

			auto signature = m_Signatures[entity];
			signature.set(GetComponentSignatureID<T>(), true);
			m_Signatures[entity] = signature;

			if (m_IsFullyInitialized) {
				EntitySignatureChanged(entity);
			}
		};

		template <typename T> void RemoveComponent(EntityID entity) {
			GetComponentArray<T>()->Remove(entity);

			auto signature = m_Signatures[entity];
			signature.set(GetComponentSignatureID<T>(), false);
			m_Signatures[entity] = signature;

			if (m_IsFullyInitialized) {
				EntitySignatureChanged(entity);
			}
		};

		template <typename T> T *GetComponent(EntityID entity) { return GetComponentArray<T>()->Get(entity); };

		template <typename T> ComponentArray<T> *GetComponentArray() {
			size_t typeID = typeid(T).hash_code();
			CKE_ASSERT(m_ComponentArrays.find(typeID) != m_ComponentArrays.end(), "Trying to add an unregistered component to an entity");
			return (ComponentArray<T> *)(m_ComponentArrays[typeID]);
		}

		// Signature
		// -----------------------------------------------------------------

		template <typename T> ComponentSignatureIndex GetComponentSignatureID() {
			size_t typeID = typeid(T).hash_code();
			CKE_ASSERT(m_ComponentSignatureIndex.find(typeID) != m_ComponentSignatureIndex.end(),
					   "A system is trying to acess a component that is not registered");
			return m_ComponentSignatureIndex[typeID];
		};

		// Systems
		// -----------------------------------------------------------------

		template <typename T> size_t RegisterSystem() {
			size_t typeID = typeid(T).hash_code();
			System *system = new T();
			system->m_Admin = this;
			m_Systems.insert({typeID, system});
			system->InitSignature();
			m_SystemsExecutionOrder.emplace_back(typeID);
			return typeID;
		};

		template <typename T> size_t RegisterSystem(T *system) {
			size_t typeID = typeid(T).hash_code();
			system->m_Admin = this;
			m_Systems.insert({typeID, system});
			system->InitSignature();
			m_SystemsExecutionOrder.emplace_back(typeID);
			return typeID;
		};

		// Views
		// -----------------------------------------------------------------

		EntitiesView *CreateView(Signature viewSignature) {
			// Check if a view exists with the same signature and
			// return it if so
			for (EntitiesView *view : m_Views) {
				if (view->m_ViewSignature == viewSignature) {
					return view;
				}
			}

			// If there isn't one then create it
			EntitiesView *view = new EntitiesView();
			view->m_ViewSignature = viewSignature;
			m_Views.push_back(view);
			return view;
		}

	private:
		TQueue<EntityID> m_AvailableEntityIDs{};
		TSet<EntityID> m_ActiveEntities{};
		TVector<Signature> m_Signatures{};
		u32 m_ActiveEntitiesCount{};

		THashMap<size_t, IComponentArray *> m_ComponentArrays{};
		THashMap<size_t, void *> m_SingletonComponents{};
		THashMap<size_t, ComponentSignatureIndex> m_ComponentSignatureIndex{};
		ComponentSignatureIndex m_NextComponentIndex{};

		THashMap<size_t, System *> m_Systems{};
		TVector<size_t> m_SystemsExecutionOrder{};

		TVector<EntitiesView *> m_Views{};

		bool m_IsFullyInitialized = false;

		// -----------------------------------------------------------------

		// Updates the entities views on each system when an entitie signature
		// has changed
		void EntitySignatureChanged(EntityID entityID) {

			Signature entitySignature = m_Signatures[entityID];
			for (EntitiesView *view : m_Views) {

				// If the view's signature has the required components we
				// add the entity to the view
				if ((view->m_ViewSignature & entitySignature) == view->m_ViewSignature) {
					view->m_Entities.insert(entityID);
				} else {
					view->m_Entities.erase(entityID);
				}
			}
		}
	};

} // namespace Cookie