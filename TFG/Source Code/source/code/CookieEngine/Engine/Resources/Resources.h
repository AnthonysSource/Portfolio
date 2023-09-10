#pragma once

#include "Core/Types/PrimitiveTypes.h"
#include "Core/Types/Containers.h"
#include "Core/Defines.h"

#include "Render/RenderAPI.h"

namespace Cookie {

	// --------------------------------------------------------------------------
	const u32 MAX_SPRITES = 5000;
	const u32 MAX_IMAGES = 5000;

	using ImageHandle = u32;
	using SpriteHandle = u32;

	// Image loaded in the CPU
	struct ImageCPU {
		i32 m_Width;
		i32 m_Height;
		i32 m_NrChannels;
		unsigned char *m_Data;
	};

	// Data used by the batch renderer
	// to render a quad with the sprite
	struct SpriteRenderData {
		f32 m_PixelsPerUnit;
		i32 m_Height;
		i32 m_Width;

		RenderingAPI::Layout m_Layout;
		RenderingAPI::Program m_Program;
		RenderingAPI::Texture m_Texture;
	};

	struct ResourcesDatabase {
		SpriteRenderData *m_Sprites[MAX_SPRITES];
		ImageCPU *m_Images[MAX_IMAGES];

		// TODO: Move this functions out and make this struct
		// internal only

		CKE_FORCE_INLINE SpriteRenderData *GetSpriteData(u32 spriteID) {
			CKE_ASSERT(m_Sprites[spriteID] != 0, "Acessing a Sprite that doesn't exist");
			return m_Sprites[spriteID];
		}

		CKE_FORCE_INLINE ImageCPU *GetImage(u32 imageID) {
			CKE_ASSERT(m_Images[imageID] != 0, "Acessing an Image that doesn't exist");
			return m_Images[imageID];
		}
	};

	// --------------------------------------------------------------------------

	extern ResourcesDatabase g_ResourcesDatabase;

	// --------------------------------------------------------------------------

	namespace ResourcesSystem {

		void Init();
		void Shutdown();

		// Sprites
		SpriteHandle GenerateSprite(ImageHandle imgHandle, f32 pixelsPerUnit);
		void DeleteSprite(SpriteHandle handle);

		// Images
		ImageHandle LoadImage(const char *path);
		void ReleaseImage(ImageHandle handle);

	} // namespace ResourcesSystem

	// --------------------------------------------------------------------------

} // namespace Cookie
