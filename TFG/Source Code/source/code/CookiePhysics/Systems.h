#pragma once

#include "Components.h"
#include "CookieEngine.h"

namespace Cookie {

	class PlayerMovementSystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<PlayerCharacterComponent>();

			m_View = g_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			for (auto const &entityID : m_View->m_Entities) {

				TransformComponent *t = g_Admin->GetComponent<TransformComponent>(entityID);
				PlayerCharacterComponent *m = g_Admin->GetComponent<PlayerCharacterComponent>(entityID);

				InputComponent *input = g_Admin->GetSinglComponent<InputComponent>();
				SinglMainPlayerComponent *mainPlayer = g_Admin->GetSinglComponent<SinglMainPlayerComponent>();

				if (input->IsKeyHeld(COOKIE_KEY_W)) {
					t->m_Position.y += m->m_Speed * dt;
				} else if (input->IsKeyHeld(COOKIE_KEY_S)) {
					t->m_Position.y -= m->m_Speed * dt;
				}

				if (input->IsKeyHeld(COOKIE_KEY_D)) {
					t->m_Position.x += m->m_Speed * dt;
				} else if (input->IsKeyHeld(COOKIE_KEY_A)) {
					t->m_Position.x -= m->m_Speed * dt;
				}

				mainPlayer->m_Position = t->m_Position;
			}
		}

	private:
		EntitiesView *m_View;
	};

	class EnemySystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<EnemyComponent>();

			m_View = g_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			auto mainPlayer = m_Admin->GetSinglComponent<SinglMainPlayerComponent>();
			auto transforms = m_Admin->GetComponentArray<TransformComponent>();
			auto enemies = m_Admin->GetComponentArray<EnemyComponent>();

			for (auto const &entityID : m_View->m_Entities) {
				TransformComponent *t = transforms->Get(entityID);
				EnemyComponent *m = enemies->Get(entityID);

				Float3 dir = mainPlayer->m_Position - t->m_Position;
				f32 distance = glm::length(dir);
				m->m_Velocity += glm::normalize(dir) * dt * m->m_Acceleration;

				// Top Speed
				if (glm::length(m->m_Velocity) > m->m_TopSpeed) {
					m->m_Velocity = glm::normalize(m->m_Velocity) * m->m_TopSpeed;
				}

				// Move out of screen if too close
				if (distance > 0.3f) {
					t->m_Position += m->m_Velocity * dt;
				} else {
					t->m_Position += Float3(10.0f, 0.0f, 0.0f);
				}
			}
		}

	private:
		EntitiesView *m_View;
	};

	class CameraSystem : public System {
	public:
		void InitSignature() {}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			CameraComponentSingl *camSingl = g_Admin->GetSinglComponent<CameraComponentSingl>();
			CameraComponent *cam = g_Admin->GetComponent<CameraComponent>(camSingl->m_MainCam);
			InputComponent *input = g_Admin->GetSinglComponent<InputComponent>();

			Float3 movementDir = Float3(0.0f, 0.0f, 0.0f);

			if (input->IsKeyHeld(COOKIE_KEY_I)) {
				movementDir.y = 1.0f;
			} else if (input->IsKeyHeld(COOKIE_KEY_K)) {
				movementDir.y = -1.0f;
			}

			if (input->IsKeyHeld(COOKIE_KEY_L)) {
				movementDir.x = 1.0f;
			} else if (input->IsKeyHeld(COOKIE_KEY_J)) {
				movementDir.x = -1.0f;
			}

			if (movementDir.x > 0.01f || movementDir.x < -0.01f || movementDir.y > 0.01f || movementDir.y < -0.01f)
				movementDir = glm::normalize(movementDir);

			movementDir.z = 0.0f;

			if (input->IsKeyHeld(COOKIE_KEY_U)) {
				movementDir.z += 1.0f;
			} else if (input->IsKeyHeld(COOKIE_KEY_O)) {
				movementDir.z -= 1.0f;
			}

			cam->m_Position += movementDir * 5.0f * dt;
		}
	};

} // namespace Cookie