#include "Loaders/MaterialLoader.h"

#include "CookieKat/Core/Memory/Memory.h"
#include "CookieKat/Systems/Resources/InstallDependencies.h"
#include "CookieKat/Engine/Resources/Resources/RenderMaterialResource.h"

namespace CKE {
	LoadResult MaterialLoader::LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const {
		RenderMaterialResource* material = New<RenderMaterialResource>();
		ar << *material;
		ctx.SetResource(material);
		return LoadResult::Successful;
	}

	LoadResult MaterialLoader::Install(LoaderContext& ctx, InstallDependencies& dependencies) {
		auto material = ctx.GetResource<RenderMaterialResource>();

		material->m_AlbedoTexture = dependencies.GetInstallDependency<RenderTextureResource>(material->m_AlbedoTexture.m_Value);
		material->m_RoughnessTexture = dependencies.GetInstallDependency<RenderTextureResource>(material->m_RoughnessTexture.m_Value);
		material->m_MetalicTexture = dependencies.GetInstallDependency<RenderTextureResource>(material->m_MetalicTexture.m_Value);
		material->m_NormalTexture = dependencies.GetInstallDependency<RenderTextureResource>(material->m_NormalTexture.m_Value);

		return LoadResult::Successful;
	}
}
