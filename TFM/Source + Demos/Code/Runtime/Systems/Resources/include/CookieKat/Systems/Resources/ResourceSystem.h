#pragma once

#include "CookieKat/Core/Containers/Containers.h"
#include "CookieKat/Core/FileSystem/FileSystem.h"

#include "CookieKat/Systems/EngineSystem/IEngineSystem.h"
#include "CookieKat/Systems/Resources/ResourceID.h"
#include "CookieKat/Systems/Resources/IResource.h"
#include "CookieKat/Systems/Resources/ResourceTypeID.h"
#include "CookieKat/Systems/Resources/ResourceLoader.h"
#include "CookieKat/Systems/Resources/ResourceRecord.h"
#include "CookieKat/Systems/TaskSystem/TaskSystem.h"

namespace CKE {
	class ResourceSystem : public IEngineSystem
	{
	public:
		// Lifetime
		//-----------------------------------------------------------------------------

		void Initialize(TaskSystem* pTaskSystem);
		void Shutdown();

		// Resource Management
		//-----------------------------------------------------------------------------

		ResourceID LoadResource(Path resourcePath);
		void       UnloadResource(ResourceID resourceID);
		IResource* GetResource(ResourceID resourceID);

		template <typename T>
			requires std::is_base_of_v<IResource, T>
		TResourceID<T> LoadResource(Path resourcePath);
		template <typename T>
			requires std::is_base_of_v<IResource, T>
		T* GetResource(ResourceID resourceID);

		// Resource Loaders
		//-----------------------------------------------------------------------------

		// The ptr to the loader must remain valid until its unregistered
		void RegisterLoader(ResourceLoader* pResourceLoader);
		void UnRegisterLoader(ResourceLoader* pResourceLoader);

		// Utils
		//-----------------------------------------------------------------------------

		Path const& GetBasePath() { return m_BaseDataPath; }
		void        SetBasePath(Path const& path) { m_BaseDataPath = path; }

	private:
		void GetResourceLoader(Path resourcePath, ResourceLoader*& pLoader);

	private:
		static constexpr u32 MAX_LOADED_RESOURCES = 25'000;

		TaskSystem* m_pTaskSystem = nullptr;

		Map<ResourceTypeID, ResourceLoader*> m_pResourceLoaders;
		Map<ResourceID, ResourceRecord>      m_pResourceDatabase;
		Map<Path, ResourceID>                m_PathToResourceID;

		Path m_BaseDataPath = "../../../../Data/";
		// Path              m_BaseDataPath = "Data/";
		Queue<ResourceID> m_AvailableResourceIDs{};
	};
}


namespace CKE {
	// Template Definitions
	//-----------------------------------------------------------------------------

	template <typename T>
		requires std::is_base_of_v<IResource, T>
	T* ResourceSystem::GetResource(ResourceID resourceID) {
		return static_cast<T*>(m_pResourceDatabase[resourceID].m_pResource);
	}

	template <typename T>
		requires std::is_base_of_v<IResource, T>
	TResourceID<T> ResourceSystem::LoadResource(Path resourcePath) {
		static_assert(std::is_base_of_v<IResource, T>);
		return TResourceID<T>{LoadResource(resourcePath)};
	}
}
