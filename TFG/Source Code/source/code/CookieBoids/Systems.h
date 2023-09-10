#pragma once

#include "Components.h"
#include "CookieEngine.h"

#include "optick.h"

namespace Cookie {

	class BoidsSystem : public System {
	public:
		const f32 visualRange = 0.05f;

		const f32 minAvoidanceDistanceSqr = 0.01f;
		const f32 avoidFactor = 0.5f;

		const f32 alignmentFactor = 0.1f;
		const f32 cohesionFactor = 0.05f;

		const f32 maxSpeed = 1.0f;

		const Float2 boundsSize = Float2(11.0f, 7.0f);
		const f32 clampToBoundsFactor = 1.0f;

		void InitSignature() {
			m_Signature.set(g_Admin->GetComponentSignatureID<TransformComponent>(), true);
			m_Signature.set(g_Admin->GetComponentSignatureID<BoidComponent>(), true);

			m_BoidsView = g_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			// Get component arrays
			ComponentArray<TransformComponent> *transfArray = g_Admin->GetComponentArray<TransformComponent>();
			ComponentArray<BoidComponent> *boidArray = g_Admin->GetComponentArray<BoidComponent>();

			for (auto const &entityID : m_BoidsView->m_Entities) {
				TransformComponent *transf = transfArray->Get(entityID);
				BoidComponent *boid = boidArray->Get(entityID);

				Float3 separationVelocity = Float3(0.0f);
				Float3 cohesionCenterOfBoids = Float3(0.0f);
				Float3 alignmentVelocity = Float3(0.0f);
				u32 numNeighbours = 0;

				for (auto const &otherEntityID : m_BoidsView->m_Entities) {
					TransformComponent *otherTransf = transfArray->Get(otherEntityID);
					BoidComponent *otherBoid = boidArray->Get(otherEntityID);

					// Avoid
					if (otherEntityID != entityID) {
						// f32 dist = glm::distance(transf->m_Position, otherTransf->m_Position);
						f32 dist =
							(transf->m_Position.x - otherTransf->m_Position.x) * (transf->m_Position.x - otherTransf->m_Position.x) +
							(transf->m_Position.y - otherTransf->m_Position.y) * (transf->m_Position.y - otherTransf->m_Position.y);
						if (dist < minAvoidanceDistanceSqr) {
							separationVelocity = transf->m_Position - otherTransf->m_Position;
						}
						if (dist < visualRange) {
							alignmentVelocity += otherBoid->m_Velocity;
							cohesionCenterOfBoids += otherTransf->m_Position;
							++numNeighbours;
						}
					}
				}

				if (numNeighbours != 0) {
					cohesionCenterOfBoids /= (f32)numNeighbours;
					alignmentVelocity /= (f32)numNeighbours;

					// Apply cohesion and alignment velocities
					boid->m_Velocity += (cohesionCenterOfBoids - transf->m_Position) * cohesionFactor;
					boid->m_Velocity += (alignmentVelocity - boid->m_Velocity) * alignmentFactor;
				}

				// Apply avoidance velocity
				boid->m_Velocity += separationVelocity * avoidFactor;

				// Clamp to screen
				if (transf->m_Position.x > boundsSize.x) {
					boid->m_Velocity -= Float3(clampToBoundsFactor, 0.0f, 0.0f) * dt;
				}
				if (transf->m_Position.x < -boundsSize.x) {
					boid->m_Velocity += Float3(clampToBoundsFactor, 0.0f, 0.0f) * dt;
				}
				if (transf->m_Position.y > boundsSize.y) {
					boid->m_Velocity -= Float3(0.0f, clampToBoundsFactor, 0.0f) * dt;
				}
				if (transf->m_Position.y < -boundsSize.y) {
					boid->m_Velocity += Float3(0.0f, clampToBoundsFactor, 0.0f) * dt;
				}

				// Limit Speed
				f32 speed = glm::length(boid->m_Velocity);
				if (speed > maxSpeed) {
					boid->m_Velocity = (boid->m_Velocity / speed) * maxSpeed;
				}

				transf->m_Position += dt * boid->m_Velocity;
				transf->m_Position.z = 0.0f;
				boid->m_Velocity.z = 0.0f;
			}
		}

	private:
		EntitiesView *m_BoidsView;
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