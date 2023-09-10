#pragma once

#include "Components.h"
#include "CookieEngine.h"

namespace Cookie {

	class PlayerMovementSystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<PlayerCharacterComponent>();

			m_View = m_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			for (auto const &entityID : m_View->m_Entities) {
				TransformComponent *t = m_Admin->GetComponent<TransformComponent>(entityID);
				PlayerCharacterComponent *m = m_Admin->GetComponent<PlayerCharacterComponent>(entityID);

				InputComponent *input = m_Admin->GetSinglComponent<InputComponent>();
				SinglMainPlayerComponent *mainPlayer = m_Admin->GetSinglComponent<SinglMainPlayerComponent>();

				Float3 movementDir = Float3(0.0f, 0.0f, 0.0f);

				if (input->IsKeyHeld(COOKIE_KEY_W)) {
					movementDir.y = 1.0f;
				} else if (input->IsKeyHeld(COOKIE_KEY_S)) {
					movementDir.y = -1.0f;
				}

				if (input->IsKeyHeld(COOKIE_KEY_D)) {
					movementDir.x = 1.0f;
				} else if (input->IsKeyHeld(COOKIE_KEY_A)) {
					movementDir.x = -1.0f;
				}

				if (movementDir.x > 0.01f || movementDir.x < -0.01f || movementDir.y > 0.01f || movementDir.y < -0.01f)
					movementDir = glm::normalize(movementDir);

				movementDir.z = 0.0f;
				t->m_Position += movementDir * m->m_Speed * dt;

				// Copy position into singleton main player component
				mainPlayer->m_Position = t->m_Position;
			}
		}

	private:
		EntitiesView *m_View;
	};

	// --------------------------------------------------------------------------

	class CameraSystem : public System {
	public:
		void InitSignature() {}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			CameraComponentSingl *camSingl = m_Admin->GetSinglComponent<CameraComponentSingl>();
			SinglMainPlayerComponent *playerSingl = m_Admin->GetSinglComponent<SinglMainPlayerComponent>();
			CameraComponent *cam = m_Admin->GetComponent<CameraComponent>(camSingl->m_MainCam);
			InputComponent *input = m_Admin->GetSinglComponent<InputComponent>();

			Float3 movementDir = Float3(0.0f, 0.0f, 0.0f);

			movementDir = playerSingl->m_Position - cam->m_Position;
			movementDir.z = 0.0f;
			f32 length = glm::length(movementDir);
			if (length > 0.0001f) {
				movementDir /= length;
			}

			cam->m_Position += movementDir * length * 5.0f * dt;
		}
	};

	// --------------------------------------------------------------------------

	class AttackSystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<AttackComponent>();
			m_AttackersView = m_Admin->CreateView(m_Signature);

			m_Signature = Signature();
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<EnemyComponent>();
			m_TargetsView = m_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			auto targetTransforms = m_Admin->GetComponentArray<TransformComponent>();
			auto attackers = m_Admin->GetComponentArray<AttackComponent>();

			Float3 enemyRespawnPos;
			if (Random::Int(0, 2) == 0) {
				enemyRespawnPos = Float3(-15.0f, 0.0f, 0.0f);
			} else {
				enemyRespawnPos = Float3(15.0f, 0.0f, 0.0f);
			}

			for (auto const &attackerID : m_AttackersView->m_Entities) {
				AttackComponent *attackerAttack = attackers->Get(attackerID);

				// Tick attack CD
				attackerAttack->m_CooldownElapsed += dt;

				if (attackerAttack->m_CooldownElapsed > attackerAttack->m_CooldownTotal) {
					// Reset Attack CD
					attackerAttack->m_CooldownElapsed = 0.0f;

					// Get Player Data
					TransformComponent *attackerTransform = targetTransforms->Get(attackerID);
					auto scoreComp = m_Admin->GetSinglComponent<ScoreSinglComponent>();

					u32 newAttackID = (attackerAttack->m_LastAttackID + 1) % 3;
					attackerAttack->m_LastAttackID = newAttackID;

					// Check the attackid and execute it
					switch (newAttackID) {
					case 0:
						for (auto const &targetID : m_TargetsView->m_Entities) {
							TransformComponent *targetTransf = targetTransforms->Get(targetID);

							// AABB
							bool inX = targetTransf->m_Position.x < attackerTransform->m_Position.x + attackerAttack->m_RectX &&
									   targetTransf->m_Position.x > attackerTransform->m_Position.x - attackerAttack->m_RectX;

							bool inY = targetTransf->m_Position.y < attackerTransform->m_Position.y + attackerAttack->m_RectY &&
									   targetTransf->m_Position.y > attackerTransform->m_Position.y - attackerAttack->m_RectY;

							if (inX && inY) {
								targetTransf->m_Position += Float3(15.0f, 0.0f, 0.0f);
								++scoreComp->m_CurrentScore;
							}
						}
						break;
					case 1:
						for (auto const &targetID : m_TargetsView->m_Entities) {
							TransformComponent *targetTransf = targetTransforms->Get(targetID);

							// AABB
							bool inX = targetTransf->m_Position.x < attackerTransform->m_Position.x + attackerAttack->m_RectY &&
									   targetTransf->m_Position.x > attackerTransform->m_Position.x - attackerAttack->m_RectY;

							bool inY = targetTransf->m_Position.y < attackerTransform->m_Position.y + attackerAttack->m_RectX &&
									   targetTransf->m_Position.y > attackerTransform->m_Position.y - attackerAttack->m_RectX;

							if (inX && inY) {
								targetTransf->m_Position += Float3(15.0f, 0.0f, 0.0f);
								++scoreComp->m_CurrentScore;
							}
						}
						break;
					case 2:
						for (auto const &targetID : m_TargetsView->m_Entities) {
							TransformComponent *targetTransf = targetTransforms->Get(targetID);
							f32 dist = glm::length(targetTransf->m_Position - attackerTransform->m_Position);
							if (dist < attackerAttack->m_MaxArea && dist > attackerAttack->m_MinArea) {
								targetTransf->m_Position += Float3(15.0f, 0.0f, 0.0f);
								++scoreComp->m_CurrentScore;
							}
						}
						break;
					}
				}
			}
		}

	private:
		EntitiesView *m_AttackersView;
		EntitiesView *m_TargetsView;
	};

	// --------------------------------------------------------------------------

	class EnemySystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<EnemyComponent>();

			m_View = m_Admin->CreateView(m_Signature);
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
				dir.z = 0.0f;
				f32 distance = glm::length(dir);
				m->m_Velocity += dir * (dt * m->m_Acceleration / distance);

				// Top Speed
				if (glm::length(m->m_Velocity) > m->m_TopSpeed) {
					m->m_Velocity = glm::normalize(m->m_Velocity) * m->m_TopSpeed;
				}

				t->m_Position += m->m_Velocity * dt;
			}
		}

	private:
		EntitiesView *m_View;
	};

} // namespace Cookie