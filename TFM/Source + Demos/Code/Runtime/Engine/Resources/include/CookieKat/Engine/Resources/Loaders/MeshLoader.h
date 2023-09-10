#pragma once

#include "CookieKat/Systems/Resources/ResourceLoader.h"

#include "CookieKat/Systems/RenderAPI/RenderDevice.h"

namespace CKE
{
	class MeshLoader : public ResourceLoader
	{
	public:
		void Initialize(RenderDevice* pRenderDevice);

		LoadResult Load(LoaderContext& ctx, Blob& binarySource) const override;
		LoadResult Install(LoaderContext& ctx, InstallDependencies& dependencies) override;

		Vector<ResourceTypeID> GetLoadableTypes() override { return{ ResourceTypeID("fbx"), ResourceTypeID("obj")}; }

	private:
		RenderDevice* m_pDevice = nullptr;
	};
}
