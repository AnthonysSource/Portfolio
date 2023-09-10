#pragma once

#include "CookieKat/Systems/Resources/ResourceLoader.h"

namespace CKE {
	class MaterialLoader : public CompiledResourcesLoader
	{
	public:
		LoadResult LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const override;
		LoadResult Install(LoaderContext& ctx, InstallDependencies& dependencies) override;

		Vector<ResourceTypeID> GetLoadableTypes() override { return{ ResourceTypeID("mat") }; }
	};
}
