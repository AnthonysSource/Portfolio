#include "EntityAdmin.h"
#include "Core/Profiling/Profiling.h"

#include "Entities/BaseComponents.h"

namespace Cookie {

	void EntityAdmin::Init() {
		// Create Entity IDs
		for (EntityID entityID = 0; entityID < MAX_ENTITIES; ++entityID) {
			m_AvailableEntityIDs.push(entityID);
			m_Signatures.emplace_back(Signature{});
		}

		// Register Base Components
		RegisterComponent<TransformComponent>();
		RegisterComponent<RenderComponent>();
		RegisterComponent<PhysicsComponent>();
		RegisterComponent<CameraComponent>();

		RegisterSinglComponent<PhysicsWorldSinglComponent>();
		RegisterSinglComponent<InputComponent>();
		RegisterSinglComponent<CameraComponentSingl>();
	}

	void EntityAdmin::InitViews() {
		for (auto entityID : m_ActiveEntities) {
			Signature signature = m_Signatures[entityID];

			for (EntitiesView *view : m_Views) {

				// If the view's signature has the required components we
				// add the entity to the view
				if ((view->m_ViewSignature & signature) == view->m_ViewSignature) {
					view->m_Entities.insert(entityID);
				} else {
					view->m_Entities.erase(entityID);
				}
			}
		}
		m_IsFullyInitialized = true;
	}

	void EntityAdmin::Update(f32 deltaTime) {
		CKE_PROFILE_EVENT();

		// Update all systems
		for (size_t i = 0; i < m_SystemsExecutionOrder.size(); i++) {
			m_Systems[m_SystemsExecutionOrder[i]]->Update(deltaTime);
		}
	}

	void EntityAdmin::Shutdown() {
		// Deallocate all systems
		for (auto const pair : m_Systems) {
			delete pair.second;
		}
		// Deallocate all component arrays
		for (auto const pair : m_ComponentArrays) {
			delete pair.second;
		}
	}

	EntityID EntityAdmin::CreateEntity() {
		CKE_ASSERT(m_ActiveEntitiesCount < MAX_ENTITIES, "Trying to create too many entities");

		// Pop an entity id from the queue
		EntityID id = m_AvailableEntityIDs.front();
		m_AvailableEntityIDs.pop();
		++m_ActiveEntitiesCount;
		m_ActiveEntities.insert(id);

		return id;
	}

	void EntityAdmin::DestroyEntity(EntityID entityID) {
		// Mark the entity id as available
		m_ActiveEntities.erase(entityID);
		m_AvailableEntityIDs.push(entityID);
		--m_ActiveEntitiesCount;

		// Clear entity signature
		m_Signatures[entityID].reset();

		// Destroy entity components
		for (auto const &pair : m_ComponentArrays) {
			IComponentArray *array = pair.second;
			array->DestroyEntity(entityID);
		}

		// Erase entity from systems iteration cache
		for (EntitiesView *view : m_Views) {
			view->m_Entities.erase(entityID);
		}
	}

} // namespace Cookie