#pragma once

#include "CookieKat/Systems/Resources/ResourceLoader.h"

#include "CookieKat/Systems/RenderAPI/RenderDevice.h"

namespace CKE {
	class PipelineLoader : public CompiledResourcesLoader
	{
	public:
		void Initialize(RenderDevice* pRenderDevice);

		LoadResult LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const override;
		LoadResult Install(LoaderContext& ctx, InstallDependencies& dependencies) override;
		LoadResult Uninstall(LoaderContext& ctx) override;

		Vector<ResourceTypeID> GetLoadableTypes() override { return{ ResourceTypeID("pipeline") }; }

	private:
		RenderDevice* m_pRenderDevice = nullptr;
	};
}
