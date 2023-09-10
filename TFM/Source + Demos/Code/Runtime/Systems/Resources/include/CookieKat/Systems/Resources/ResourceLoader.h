#pragma once

#include "CookieKat/Core/Containers/Containers.h"
#include "CookieKat/Core/FileSystem/FileSystem.h"

#include "CookieKat/Systems/Resources/ResourceID.h"
#include "CookieKat/Systems/Resources/IResource.h"

namespace CKE {
	class InstallDependencies;
}

namespace CKE {
	enum class LoadResult : bool
	{
		Successful = true,
		Failed = false
	};

	class LoaderContext
	{
	public:
		inline IResource* GetResource() { return m_pResource; }
		inline void       SetResource(IResource* resource) { m_pResource = resource; }

		template <typename T>
		inline T* GetResource() { return reinterpret_cast<T*>(m_pResource); }

		//-----------------------------------------------------------------------------

		inline ResourceID GetResourceID() { return m_ID; }

		//-----------------------------------------------------------------------------

		inline void AddDependency(Path const& dependencyPath) { m_Dependencies.emplace_back(dependencyPath); }

	private:
		friend class ResourceSystem;
		ResourceID   m_ID;                  // Runtime identifier in the database
		Path         m_AssetPath;           // Unique Asset identifier and path of the resource in the file system
		IResource*   m_pResource = nullptr; // Ptr to the resource
		Vector<Path> m_Dependencies;        // Resources that *are used* by this resource
	};

	//-----------------------------------------------------------------------------

	// Base Interface for all of the loaders in the resource system
	class ResourceLoader
	{
	public:
		//-----------------------------------------------------------------------------

		// Handles loading the resource
		virtual LoadResult Load(LoaderContext& ctx, Vector<u8>& binarySrc) const { return LoadResult::Successful; }

		// Allows Post-Loading logic for the resource if necessary
		virtual LoadResult Install(LoaderContext& ctx, InstallDependencies& dependencies) { return LoadResult::Successful; }

		// Allows Pre-Unloading logic for the resource if necessary
		virtual LoadResult Uninstall(LoaderContext& ctx) { return LoadResult::Successful; }

		// Handles unloading the resource
		virtual LoadResult Unload(LoaderContext& ctx) const { return LoadResult::Successful; }

		//-----------------------------------------------------------------------------

		// Returns the loadable resource types that the loader will handle
		virtual Vector<ResourceTypeID> GetLoadableTypes() = 0;

		//-----------------------------------------------------------------------------

		virtual ~ResourceLoader() = default;
	};

	class CompiledResourcesLoader : public ResourceLoader
	{
	public:
		virtual LoadResult LoadCompiledResource(LoaderContext& ctx, BinaryInputArchive& ar) const = 0;

	private:
		virtual LoadResult Load(LoaderContext& ctx, Vector<u8>& binarySrc) const override;
	};
}

namespace CKE {
	inline LoadResult CompiledResourcesLoader::Load(LoaderContext& ctx, Vector<u8>& binarySrc) const {
		BinaryInputArchive ar;
		ar.ReadFromBlob(binarySrc);

		ResourceHeader header;
		ar << header;

		for (Path& dependency : header.m_DependencyPaths) {
			ctx.AddDependency(dependency);
		}

		LoadCompiledResource(ctx, ar);

		return LoadResult::Successful;
	}
}
