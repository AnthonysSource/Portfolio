#include "PhysicsSystem.h"
#include "Core/Application.h"

#include "Entities/Components/TransformComponent.h"
#include "Entities/Components/PhysicsComponent.h"
#include "Entities/EntityAdmin.h"

#include <box2d/box2d.h>

namespace Cookie {

	void PhysicsSystem::InitSignature() {
		// Create Physics World
		auto physicsWorldComp = m_Admin->GetSinglComponent<PhysicsWorldSinglComponent>();

		b2Vec2 gravity(0.0f, -10.0f);
		physicsWorldComp->m_World = new b2World(gravity);

		// Create View
		SetRequiredComponent<TransformComponent>();
		SetRequiredComponent<PhysicsComponent>();
		m_View = m_Admin->CreateView(m_Signature);
	}

	// TODO: Fixed Time-Step
	void PhysicsSystem::Update(f32 dt) {
		auto physicsWorldComp = m_Admin->GetSinglComponent<PhysicsWorldSinglComponent>();
		physicsWorldComp->m_World->Step(dt, 8, 3);

		auto transforms = g_Admin->GetComponentArray<TransformComponent>();
		auto physicsComp = g_Admin->GetComponentArray<PhysicsComponent>();

		for (auto &entityID : m_View->m_Entities) {
			auto t = transforms->Get(entityID);
			auto p = physicsComp->Get(entityID);

			b2Vec2 pos = p->m_Body->GetPosition();
			t->m_Position = Float3(pos.x, pos.y, t->m_Position.z);
		}
	}

	void PhysicsSystem::Shutdown() {
		// Deallocate physics world
		auto physicsWorldComp = m_Admin->GetSinglComponent<PhysicsWorldSinglComponent>();
		delete physicsWorldComp->m_World;
	}

	class b2Contacts : public b2ContactListener {
		void BeginContact(b2Contact *contact) override {}

		/// Called when two fixtures cease to touch.
		void EndContact(b2Contact *contact) override {}
	};

} // namespace Cookie
