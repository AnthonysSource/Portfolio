#pragma once

#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Cookie {

	template <typename T> using TVector = std::vector<T>;
	template <typename T, size_t S> using TArray = std::array<T, S>;
	template <typename T> using TSet = std::unordered_set<T>;
	template <typename T> using TQueue = std::queue<T>;
	template <typename K, typename V> using THashMap = std::unordered_map<K, V>;

	template <size_t S> using Bitset = std::bitset<S>;

} // namespace Cookie