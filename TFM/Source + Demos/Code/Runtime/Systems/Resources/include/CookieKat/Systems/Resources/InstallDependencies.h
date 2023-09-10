#pragma once

#include "CookieKat/Core/Containers/Containers.h"

namespace CKE
{
	// Wrapper for a list of resources that need to be installed before
	// a specific resource is installed.
	//
	// This list is populated by the resource system and is passed to the
	// public loader when the dependencies have already been installed.
	// The user can acquire these dependencies by calling the class methods
	class InstallDependencies
	{
		friend class ResourceSystem;

	public:
		template <typename T>
			requires std::is_base_of_v<IResource, T>
		TResourceID<T> GetInstallDependency(u64 dependencyIndexInResourceHeader) const
		{
			TResourceID<T> r{};
			r.m_Value = m_DependencyIDs[dependencyIndexInResourceHeader].m_Value;
			return r;
		}

	private:
		Vector<ResourceID> m_DependencyIDs;
	};
}
