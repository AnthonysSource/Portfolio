#include "RenderingSystem.h"

#include "Core/Application.h"
#include "Core/Profiling/Profiling.h"
#include "Core/Math/Math.h"

#include "Render/RenderAPI.h"
#include "Render/IMGUI/IMGUI_Impl.h"

#include "Resources/Resources.h"

#include "Entities/EntityAdmin.h"
#include "Entities/BaseComponents.h"

namespace Cookie {

	using namespace RenderingAPI;

	// Batch rendering
	// --------------------------------------------------------------------------

	struct RenderBatch {
		VertexBuffer m_VertexBuffer;
		IndexBuffer m_IndexBuffer;
		Layout m_Layout;
		Program m_Program;
		Texture m_Texture;

		// Camera that will be used for rendering
		CameraComponent *m_Cam;

		// Num of sprites that will be drawn in the batch
		u32 m_NumSpritesToDraw;

		// Max number of sprites that this batch will contain
		u32 m_MaxSpritesToDraw;

		// Vertex data of all of the quads/sprites
		Vertex *m_Vertices;
		u32 *m_Indices;
	};

	CKE_FORCE_INLINE void AddQuad(RenderBatch *batch, Float3 pos, u32 width, u32 height, f32 pixelsPerUnit) {

		batch->m_Vertices[batch->m_NumSpritesToDraw * 4 + 0] = Vertex(
			Float3(-(f32)width / (2.0f * pixelsPerUnit) + pos.x, -(f32)height / (2.0f * pixelsPerUnit) + pos.y, pos.z), Float2(0.0f, 0.0f));
		batch->m_Vertices[batch->m_NumSpritesToDraw * 4 + 1] = Vertex(
			Float3((f32)width / (2.0f * pixelsPerUnit) + pos.x, -(f32)height / (2.0f * pixelsPerUnit) + pos.y, pos.z), Float2(1.0f, 0.0f));
		batch->m_Vertices[batch->m_NumSpritesToDraw * 4 + 2] = Vertex(
			Float3((f32)width / (2.0f * pixelsPerUnit) + pos.x, (f32)height / (2.0f * pixelsPerUnit) + pos.y, pos.z), Float2(1.0f, 1.0f));
		batch->m_Vertices[batch->m_NumSpritesToDraw * 4 + 3] = Vertex(
			Float3(-(f32)width / (2.0f * pixelsPerUnit) + pos.x, (f32)height / (2.0f * pixelsPerUnit) + pos.y, pos.z), Float2(0.0f, 1.0f));

		++batch->m_NumSpritesToDraw;
	}

	void FlushBatch(RenderBatch *batch) {
		Matrix4 view = glm::lookAt(batch->m_Cam->m_Position, Float3(batch->m_Cam->m_Position.x, batch->m_Cam->m_Position.y, 0.0f),
								   Float3(0.0f, 1.0f, 0.0f));
		Matrix4 proj = glm::perspective(45.0f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		//Matrix4 proj = glm::ortho(-5.0f, +5.0f, -2.5f, +2.5f, 0.001f, 1000.0f);
		Matrix4 model = Matrix4(1.0f);

		// Setup and Submit Drawcall
		Context::BindProgram(&batch->m_Program);

		batch->m_Program.SetUniformMat4("view", glm::value_ptr(view));
		batch->m_Program.SetUniformMat4("projection", glm::value_ptr(proj));
		batch->m_Program.SetUniformMat4("model", glm::value_ptr(model));

		Device::BufferDataIntoVertexBuffer(&batch->m_VertexBuffer, (const char *)batch->m_Vertices,
										   batch->m_NumSpritesToDraw * 4 * sizeof(Vertex));
		Context::BindVertexBuffer(&batch->m_VertexBuffer);
		Context::BindIndexBuffer(&batch->m_IndexBuffer);
		Context::BindTexture(&batch->m_Texture);
		Context::BindLayout(&batch->m_Layout);
		Context::Submit(batch->m_NumSpritesToDraw);

		// Reset Batch
		batch->m_NumSpritesToDraw = 0;
	}

	void DeleteBatch(RenderBatch *batch) {
		delete batch->m_Indices;
		delete batch->m_Vertices;
	}

	// --------------------------------------------------------------------------

	RenderBatch batch = {};

	// We store a vector with all the entities
	// that are using the same sprite to batch render them
	THashMap<SpriteHandle, TVector<EntityID>> m_SortedEntitiesMap = {};

	// Rendering System Lifetime
	// --------------------------------------------------------------------------

	void RenderingSystem::InitSignature() {
		// SetRequiredComponent<TransformComponent>();
		// SetRequiredComponent<RenderComponent>();

		m_Signature.set(g_Admin->GetComponentSignatureID<TransformComponent>(), true);
		m_Signature.set(g_Admin->GetComponentSignatureID<RenderComponent>(), true);

		m_RendereableView = g_Admin->CreateView(m_Signature);
	}

	void RenderingSystem::Init() {
		Context::Init();

		// Init IMGUI
		ImGuiRenderer::Init();

		// Init Batch Rendering
		u32 MAX_SPRITES = 5000;
		u32 MAX_INDICES = MAX_SPRITES * 6;
		u32 MAX_VERTICES = MAX_SPRITES * 4;

		batch.m_MaxSpritesToDraw = MAX_SPRITES;
		batch.m_VertexBuffer = Device::CreateDynamicVertexBuffer(MAX_SPRITES);

		batch.m_Indices = new u32[MAX_INDICES];
		batch.m_Vertices = new Vertex[MAX_VERTICES];
		u32 offset = 0;
		for (size_t i = 0; i < MAX_INDICES; i += 6) {
			batch.m_Indices[i + 0] = 0 + offset;
			batch.m_Indices[i + 1] = 1 + offset;
			batch.m_Indices[i + 2] = 2 + offset;

			batch.m_Indices[i + 3] = 2 + offset;
			batch.m_Indices[i + 4] = 3 + offset;
			batch.m_Indices[i + 5] = 0 + offset;

			offset += 4;
		}

		batch.m_IndexBuffer = Device::CreateIndexBuffer((char *)batch.m_Indices, sizeof(u32) * MAX_INDICES, DataType::UINT);
	}

	void RenderingSystem::Shutdown() {
		ImGuiRenderer::Shutdown();

		DeleteBatch(&batch);
	}

	// --------------------------------------------------------------------------

	void RenderingSystem::Update(f32 dt) {
		CKE_PROFILE_EVENT();
		// Clear buffer
		Context::ClearBuffer(0.95f, 0.6f, 0.05f, 1.0f);

		// NOTE: Current code implements "layers" by using the Z coordinate
		// On Initialization:
		//		Group all entities by sprite
		// NOTE: Ideally only update the groups when a new rendereable entity is created
		//
		// On Update
		//		Discard all entities outside of the camera view
		//			For all sprite groups
		//				Create a Batch
		//				Add Sprite Quads
		//				Submit Batch Drawcall
		//
		u32 numBatches = 0;

		auto camSingl = g_Admin->GetSinglComponent<CameraComponentSingl>();

		auto transforms = g_Admin->GetComponentArray<TransformComponent>();
		auto render = g_Admin->GetComponentArray<RenderComponent>();
		auto renderCam = g_Admin->GetComponent<CameraComponent>(camSingl->m_MainCam);

		batch.m_Cam = renderCam;

		for (auto &sortedEntities : m_SortedEntitiesMap) {
			sortedEntities.second.clear();
		}

		for (EntityID const &entityID : m_RendereableView->m_Entities) {
			RenderComponent *r = render->Get(entityID);
			m_SortedEntitiesMap[r->m_SpriteHandle].emplace_back(entityID);
		}

		for (auto sortedEntities : m_SortedEntitiesMap) {
			TVector<EntityID> entitiesArray = sortedEntities.second;
			for (size_t i = 0; i < entitiesArray.size(); i++) {
				EntityID entityID = entitiesArray[i];

				TransformComponent *t = transforms->Get(entityID);
				RenderComponent *r = render->Get(entityID);

				SpriteRenderData *sp = g_ResourcesDatabase.GetSpriteData(r->m_SpriteHandle);

				// This approach means that we don't guarantee drawing
				// all the sprites with the same texture in the same batch
				if (batch.m_Texture.m_ID != sp->m_Texture.m_ID) {
					if (batch.m_NumSpritesToDraw > 0) {
						FlushBatch(&batch);
						++numBatches;
					}
					batch.m_Texture = sp->m_Texture;
					batch.m_Layout = sp->m_Layout;
					batch.m_Program = sp->m_Program;
				}

				if (batch.m_NumSpritesToDraw >= batch.m_MaxSpritesToDraw - 1) {
					FlushBatch(&batch);
					++numBatches;
					batch.m_Texture = sp->m_Texture;
					batch.m_Layout = sp->m_Layout;
					batch.m_Program = sp->m_Program;
				}

				AddQuad(&batch, t->m_Position, sp->m_Width, sp->m_Height, sp->m_PixelsPerUnit);
			}
		}

		FlushBatch(&batch);
		++numBatches;

		// IMGUI Rendering
		ImGuiRenderer::NewFrame();
		ImGui::LabelText("FPS", "%f", 1.0f / dt);
		ImGui::LabelText("FrameTime", "%f ms", (f64)dt * 1000.0f);
		ImGui::LabelText("Batches", "%d", numBatches);
		ImGuiRenderer::Render();

		Context::SwapBuffers();
	}

} // namespace Cookie