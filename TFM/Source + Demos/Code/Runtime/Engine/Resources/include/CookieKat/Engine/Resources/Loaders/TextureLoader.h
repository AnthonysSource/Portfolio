#pragma once

#include "CookieKat/Core/FileSystem/FileSystem.h"

#include "CookieKat/Systems/RenderAPI/RenderDevice.h"
#include "CookieKat/Systems/Resources/ResourceLoader.h"

namespace CKE {
	class TextureLoader : public CompiledResourcesLoader
	{
	public:
		void Initialize(RenderDevice* pRenderDevice);

		LoadResult LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const override;
		LoadResult Install(LoaderContext& ctx, InstallDependencies& dependencies) override;
		LoadResult Uninstall(LoaderContext& ctx) override;

		Vector<ResourceTypeID> GetLoadableTypes() override { return {ResourceTypeID("tex")}; }

	private:
		RenderDevice* m_pRenderDevice = nullptr;
	};

	class CubeMapLoader : public CompiledResourcesLoader
	{
	public:
		void Initialize(RenderDevice* pRenderDevice);

		LoadResult LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const override;
		LoadResult Install(LoaderContext& ctx, InstallDependencies& dependencies) override;
		LoadResult Uninstall(LoaderContext& ctx) override;

		Vector<ResourceTypeID> GetLoadableTypes() override { return {ResourceTypeID("cubeMap")}; }

	private:
		RenderDevice* m_pRenderDevice = nullptr;
	};
}
