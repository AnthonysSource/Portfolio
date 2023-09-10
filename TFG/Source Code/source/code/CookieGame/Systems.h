#pragma once

#include "Components.h"
#include "CookieEngine.h"

#include "optick.h"

namespace Cookie {

	class RotateSystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<RotatingComponent>();

			m_View = g_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			// auto entities = GetView<TransformComponent, const RotatingComponent>()
			auto transforms = g_Admin->GetComponentArray<TransformComponent>();
			auto rotations = g_Admin->GetComponentArray<RotatingComponent>();

			for (auto const &entityID : m_View->m_Entities) {
				TransformComponent *t = transforms->Get(entityID);
				RotatingComponent const *f = rotations->Get(entityID);
				t->m_Rotation += f->m_Speed* dt;
			}
		}

	private:
		EntitiesView *m_View;
	};

	class PlayerMovementSystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<PlayerCharacterComponent>();

			m_View = g_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			auto transforms = g_Admin->GetComponentArray<TransformComponent>();
			auto players = g_Admin->GetComponentArray<PlayerCharacterComponent>();
			InputComponent *input = g_Admin->GetSinglComponent<InputComponent>();

			for (auto const &entityID : m_View->m_Entities) {

				TransformComponent *t = transforms->Get(entityID);
				PlayerCharacterComponent *m = players->Get(entityID);

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
			}
		}

	private:
		EntitiesView *m_View;
	};

	class FloatSystem : public System {
	public:
		void InitSignature() {
			SetRequiredComponent<TransformComponent>();
			SetRequiredComponent<FloatComponent>();

			m_View = g_Admin->CreateView(m_Signature);
		}

		void Update(f32 dt) override {
			CKE_PROFILE_EVENT();

			auto transforms = g_Admin->GetComponentArray<TransformComponent>();
			auto floats = g_Admin->GetComponentArray<FloatComponent>();

			for (auto const &entityID : m_View->m_Entities) {
				TransformComponent *t = transforms->Get(entityID);
				FloatComponent *f = floats->Get(entityID);
				t->m_Position.y = f->m_Amplitude * (float)cos(f->m_Speed * g_EngineClock.m_TimeData.m_SecondsUpTime);
			}
		}

	private:
		EntitiesView *m_View;
	};

} // namespace Cookie