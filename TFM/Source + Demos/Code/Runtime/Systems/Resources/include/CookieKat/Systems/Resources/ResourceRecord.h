#pragma once

#include "CookieKat/Core/Containers/Containers.h"

#include "CookieKat/Systems/Resources/ResourceID.h"
#include "CookieKat/Systems/Resources/IResource.h"

namespace CKE {
	struct ResourceRecord
	{
		ResourceID   m_ID;                  // Runtime identifier in the database
		Path         m_Path;                // Unique identifier and resource path in the file system
		IResource*   m_pResource = nullptr; // Ptr to the resource
		Vector<Path> m_Dependencies;        // Resources that *are used* by this resource
		Vector<Path> m_Users;               // Resources that *use* this resource
	};
}
