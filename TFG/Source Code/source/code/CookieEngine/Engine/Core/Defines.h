#pragma once

#include "Core/Logging/Log.h"

namespace Cookie {

//-------------------------------------------------------------------------

// Asserts
#ifdef CKE_ASSERTS
	#include <cstdlib>
	#define CKE_ASSERT(condition, ...)                                                                                                     \
		do {                                                                                                                               \
			if (!(condition)) {                                                                                                            \
				CKE_LOG_INFO(Log::Channel::Core, __VA_ARGS__);                                                                             \
				abort();                                                                                                                   \
			}                                                                                                                              \
		} while (0)
#else
	#define CKE_ASSERT(x, msg)
#endif

	//-------------------------------------------------------------------------

#define CKE_FORCE_INLINE __forceinline // MSVC Force inline
#define CKE_BREAK() abort()

} // namespace Cookie