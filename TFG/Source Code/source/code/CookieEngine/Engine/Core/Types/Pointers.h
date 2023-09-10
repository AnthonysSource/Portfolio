#pragma once

#include <memory>

namespace Cookie {

	template <typename T> using TSharedPtr = std::shared_ptr<T>;
	template <typename T> using TUniquePtr = std::unique_ptr<T>;

} // namespace Cookie