#include "ResourceSystem.h"

#include "CookieKat/Core/Platform/PlatformTime.h"
#include "CookieKat/Core/Logging/LoggingSystem.h"

#include "CookieKat/Systems/Resources/InstallDependencies.h"

#include <chrono>

namespace CKE {
	void ResourceSystem::Initialize(TaskSystem* pTaskSystem) {
		// Generate possible runtime resource IDs
		for (u64 i = 1; i <= MAX_LOADED_RESOURCES; ++i) {
			m_AvailableResourceIDs.push(ResourceID{i});
		}

		m_pTaskSystem = pTaskSystem;
	}

	void ResourceSystem::Shutdown() { }

	void ResourceSystem::GetResourceLoader(Path resourcePath, ResourceLoader*& pLoader) {
		u64                  i = resourcePath.find_last_of('.');
		String               extension = resourcePath.substr(i + 1);
		const ResourceTypeID resTypeID{extension};

		auto const loaderPair = m_pResourceLoaders.find(resTypeID);
		if (loaderPair == m_pResourceLoaders.end()) {
			std::cout << "ERROR: Couldn't find a loader for the given extension [" << extension << "]\n";
			CKE_UNREACHABLE_CODE();
		}
		pLoader = loaderPair->second;
	}

	ResourceID ResourceSystem::LoadResource(Path resourcePath) {
		auto startTime = std::chrono::system_clock::now();

		// Check if its already loaded and return if so
		//-----------------------------------------------------------------------------

		if (m_PathToResourceID.contains(resourcePath)) {
			// std::cout <<
			//		"WARNING: Trying to load an already loaded resource, the existing one has been returned instead of loading a new one.\n";
			return m_PathToResourceID[resourcePath];
		}

		// Create a record
		//-----------------------------------------------------------------------------

		CKE_ASSERT(m_AvailableResourceIDs.size() > 0); // If this isn't true then we ran out of IDs

		ResourceRecord record{};
		record.m_Path = resourcePath;
		record.m_ID = m_AvailableResourceIDs.front();
		m_AvailableResourceIDs.pop();

		m_PathToResourceID.insert({resourcePath, record.m_ID});

		// Get file extension from path and search the loader for the given type
		//-----------------------------------------------------------------------------

		ResourceLoader* pLoader;
		GetResourceLoader(resourcePath, pLoader);
		CKE_ASSERT(pLoader != nullptr); // We haven't found a loader for the given resource type

		// Load binary data
		//-----------------------------------------------------------------------------

		String const fullPath = m_BaseDataPath + resourcePath;
		Blob         blob = g_FileSystem.ReadBinaryFile(fullPath);

		// Load Resource and dependencies
		//-----------------------------------------------------------------------------

		LoaderContext loaderContext{};
		loaderContext.m_AssetPath = record.m_Path;
		loaderContext.m_ID = record.m_ID;
		pLoader->Load(loaderContext, blob);
		CKE_ASSERT(loaderContext.GetResource() != nullptr);
		record.m_Dependencies = loaderContext.m_Dependencies;
		record.m_pResource = loaderContext.m_pResource;

		// Load and install dependencies
		InstallDependencies installDependencies{};
		for (Path const& dependencyPath : record.m_Dependencies) {
			ResourceID dependencyID = LoadResource(dependencyPath);

			// Add user to child resource
			ResourceRecord& dependencyRecord = m_pResourceDatabase[dependencyID];
			dependencyRecord.m_Users.push_back(record.m_Path);

			// Save install dependencies 
			installDependencies.m_DependencyIDs.emplace_back(dependencyID);
		}

		// Install parent resource
		pLoader->Install(loaderContext, installDependencies);

		// Insert it into the database (We insert it last because we copy it as value)
		m_pResourceDatabase.insert({record.m_ID, record});

		// Time to load tracking
		auto endTime = std::chrono::system_clock::now();
		auto elapsed =
				std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
		g_LoggingSystem.Log(LogLevel::Info, LogChannel::Assets, "Loaded {} / Time: {}ms\n", record.m_Path,
		                    elapsed.count());

		return record.m_ID;
	}

	void ResourceSystem::UnloadResource(ResourceID resourceID) {
		//CKE_ASSERT(m_pResourceDatabase.contains(resourceID));
		//ResourceRecord& record = m_pResourceDatabase[resourceID];
		//if(!record.m_Users.empty()) {
		//	g_LoggingSystem.Log(LogLevel::Warning, LogChannel::Assets, "Trying to unload a resource with users");
		//	return;
		//}

		//// Update path to ID link
		//Path path{};
		//for (auto& [p, id] : m_PathToResourceID) {
		//	if (id == resourceID) {
		//		m_PathToResourceID.erase(p);
		//		path = p;
		//		break;
		//	}
		//}

		//// Get Loader
		//ResourceLoader* pLoader;
		//GetResourceLoader(path, pLoader);
		//CKE_ASSERT(pLoader != nullptr); // We haven't found a loader for the given resource type

		//pLoader->Uninstall(record);
		//CKE_ASSERT(record.GetResource() != nullptr);

		//for (Path& dependencyPath : record.m_Dependencies) {
		//	ResourceID dependencyID = LoadResource(dependencyPath);
		//	ResourceRecord& dependencyRecord = m_pResourceDatabase[dependencyID];

		//	// Remove this user from the dependency
		//	Vector<Path>& deps = dependencyRecord.m_Users;
		//	for (int i = 0; i < deps.size(); ++i) {
		//		if (deps[i] == path) {
		//			deps[i] = deps.back();
		//			deps.pop_back();
		//		}
		//	}

		//	// If there are no more users of the dependency, unload it
		//	if(deps.empty()) {
		//		UnloadResource(dependencyID);
		//	}
		//}

		//// Unload parent resource
		//pLoader->Unload(record);

		//// Return the ID as available
		//m_AvailableResourceIDs.push(resourceID);
		//resourceID.m_ID = 0;
	}

	IResource* ResourceSystem::GetResource(ResourceID resourceID) {
		IResource* res = m_pResourceDatabase[resourceID].m_pResource;
		CKE_ASSERT(res != nullptr);
		return res;
	}

	void ResourceSystem::RegisterLoader(ResourceLoader* pResourceLoader) {
		for (ResourceTypeID resTypeID : pResourceLoader->GetLoadableTypes()) {
			// Check that there are not already loaders registered for a given type
			CKE_ASSERT(!m_pResourceLoaders.contains(resTypeID));
			m_pResourceLoaders.insert({resTypeID, pResourceLoader});
		}
	}

	void ResourceSystem::UnRegisterLoader(ResourceLoader* pResourceLoader) {
		for (ResourceTypeID resTypeID : pResourceLoader->GetLoadableTypes()) {
			if (m_pResourceLoaders.contains(resTypeID)) {
				// TODO: replace ptr comparisong with loader type ID
				if (m_pResourceLoaders.at(resTypeID) == pResourceLoader) {
					m_pResourceLoaders.erase(resTypeID);
				}
			}
		}
	}
}
