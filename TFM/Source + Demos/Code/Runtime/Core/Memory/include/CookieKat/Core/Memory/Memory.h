#pragma once

#include "CookieKat/Core/Platform/Asserts.h"
#include "CookieKat/Core/Platform/PrimitiveTypes.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "CookieKat/Core/Containers/Containers.h"

// Module Contents
//-----------------------------------------------------------------------------
/*
Smart Pointers
  FullySafe
	OwningPtr
	RefCountedPtr
	Atomically Ref Counted
	Nullable
	[NO] Weak Ptr (Unowning)

Base Allocators
	Block Alloc/Free
	Individual Typed New/Delete
	Array Typed New/Delete

Memory Managers
	LinearAllocator
	StackAllocator
	PoolAllocator

Debugging Toggles
	[NO] Statistics Logging
	[NO] Asserts
*/

namespace {
	using namespace CKE;

	// Settings
	//-----------------------------------------------------------------------------

	// NOTE: General asserts must be enabled in the build system for this to work
	constexpr bool CKE_MEMORY_ASSERT_ENABLED = true;
	// Toggle logging all of the memory operations realized
	constexpr bool CKE_MEMORY_LOG = false;
	// Toggle storing a history of all memory operations (TODO: NOT WORKING, IN DEV.)
	constexpr bool CKE_MEMORY_TRACK = true;

	constexpr usize DEFAULT_ALLOC_ALIGNMENT{8};

	//-----------------------------------------------------------------------------

	// Defines all of the memory core operations
	enum class MemoryOp : i32
	{
		Alloc = 0,
		Free = 1,
		New = 2,
		Delete = 3,
		NewArray = 4,
		DeleteArray = 5,
	};

	// String version of all of the memory operations enum
	constexpr const char* MemoryOpStrings[] = {
		"Alloc",
		"Free",
		"New",
		"Delete",
		"NewArray",
		"DeleteArray"
	};

	// General purpose logging function for memory operations
	CKE_FORCE_INLINE void MemoryLog(void* pAddress, MemoryOp op, const char* type, u32 size, u32 alignment) {
		std::cout << "Memory Op: " << MemoryOpStrings[(i32)op] << " / ";
		std::cout << "Type: " << type << " / ";
		std::cout << "Address: " << pAddress << " / ";
		std::cout << "Size: " << size << " / ";
		std::cout << "Alignment: " << alignment << " / ";
		std::cout << "\n";
	}

	// Templated shortcut version of memory logging functions
	template <typename T>
	CKE_FORCE_INLINE void MemoryLog(void* pAddress, MemoryOp op) {
		MemoryLog(pAddress, op, typeid(T).name(), sizeof(T), alignof(T));
	}

	// Defines data related to a memory allocation
	struct MemoryAllocationInfo
	{
		const char* m_Type;
		void*       m_pAddress;
		u32         m_Size;
		u32         m_Alignment;
	};

	// Stores statistics about all of the tracked memory operations
	class MemoryTracking
	{
	public:
		u64                               m_TotalAllocated{0};
		u64                               m_TotalFreed{0};
		Array<MemoryAllocationInfo, 2500> m_AllocationInfo;
	};

	inline MemoryTracking g_MemoryTracking{};
}

namespace CKE {
	// Basic Memory Operations
	//-----------------------------------------------------------------------------

	// Checks if a memory address is aligned to the provided alignment
	inline bool IsAligned(void const* pMemory, usize const alignment) {
		return (reinterpret_cast<uintptr_t>(pMemory) % alignment) == 0;
	}

	// Checks if a memory address is aligned to the alignment of T
	template <typename T>
	inline bool IsAligned(T* pMemory) {
		return IsAligned(pMemory, alignof(T));
	}

	// Alloc & Free
	//-----------------------------------------------------------------------------

	// Allocates an aligned continuous memory block of a given size
	//
	// Example:
	//     i32* pMemBlock = reinterpret_cast<i32*>(CKE::Alloc(sizeof(i32)*5));
	[[nodiscard]] CKE_FORCE_INLINE void* Alloc(u64 size, u64 alignment = DEFAULT_ALLOC_ALIGNMENT) {
		void* pMemoryBlock = _aligned_malloc(size, alignment);
		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pMemoryBlock != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog(pMemoryBlock, MemoryOp::Alloc, "void", size, alignment); }
		return pMemoryBlock;
	}

	// Frees an aligned continuous memory block and nullifies the provided pointer
	// Must only be used with memory allocated by CKE::Alloc()
	//
	// Example:
	//     i32* pMemBlock = reinterpret_cast<i32*>(CKE::Alloc(sizeof(i32)*5));
	//     CKE::Free(pMemBlock);
	//     CKE_ASSERT(pMemBlock == nullptr);
	CKE_FORCE_INLINE void Free(void* pMemoryBlock) {
		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pMemoryBlock != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog(pMemoryBlock, MemoryOp::Free, "void", 0, 0); }
		_aligned_free(pMemoryBlock);
	}

	// New & Delete
	//-----------------------------------------------------------------------------

	// Allocates and calls the constructor of a T object
	//
	// Example:
	//     T* pConstructedObj = CKE::New("Constructor Arg", 232, 53.2f);
	template <typename T, typename... ConstructorArgs>
		requires std::is_constructible_v<T, ConstructorArgs...>
	[[nodiscard]] CKE_FORCE_INLINE T* New(ConstructorArgs&&... args) {
		void* pMemoryBlock = Alloc(sizeof(T), alignof(T));
		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pMemoryBlock != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog<T>(pMemoryBlock, MemoryOp::New); };
		return new(pMemoryBlock) T(std::forward<ConstructorArgs>(args)...);
	}

	// Constructs a T object in the supplied memory address (C++ placement new)
	// Doesn't check if T is aligned in memory
	// Don't pass a memory block smaller than sizeof(T)
	//
	// Example:
	//     void* pMemBlock = CKE::Alloc(sizeof(T));
	//     CKE::NewInPlace(pMemBlock, "Constructor Arg", 232, 53.2f);
	template <typename T, typename... ConstructorArgs>
		requires std::is_constructible_v<T, ConstructorArgs...>
	[[nodiscard]] CKE_FORCE_INLINE T* NewInPlace(void* pMemoryBlock, ConstructorArgs&&... args) {
		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pMemoryBlock != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog<T>(pMemoryBlock, MemoryOp::New); };
		return new(pMemoryBlock) T(std::forward<ConstructorArgs>(args)...);
	}

	// Calls the destructor of the T object and frees the memory
	// Must only be called with memory allocated by CKE::New()
	template <typename T>
	CKE_FORCE_INLINE void Delete(T*& pMemoryBlock) {
		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pMemoryBlock != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog<T>(pMemoryBlock, MemoryOp::Delete); };
		pMemoryBlock->~T();
		Free(pMemoryBlock);
	}

	// New & Delete Arrays
	//-----------------------------------------------------------------------------

	// Allocates an array of "count" elements and calls their constructor
	//
	// Example:
	//     u32* arr = CKE::NewArray(10, 0);
	//     arr[3] = 2;
	template <typename T, typename... ConstructorArgs>
	[[nodiscard]] T* NewArray(usize count, ConstructorArgs&&... args) {
		usize constexpr alignment = alignof(T);

		// We store the array count to call the destructors on all elements in DeleteArray()
		usize constexpr paddingForArrayCount = sizeof(usize);

		// We allocate extra memory to store the array count.
		// The extra memory size has to be chosen so that the rest of the block is still aligned.
		// (pBaseMemoryAddress + pExtraMemory -> Aligned)
		usize constexpr extraMemoryInBytes = std::max(alignment, paddingForArrayCount);

		// Allocate block
		usize const totalBlockSize = count * sizeof(T) + extraMemoryInBytes;
		u8*         pArrayBase = static_cast<u8*>(Alloc(totalBlockSize, alignment));

		// Write the array count at the start of the memory block
		*reinterpret_cast<usize*>(pArrayBase) = count;

		// Get array ptr to its elements data section and call all of the constructors
		T* pArrayData = reinterpret_cast<T*>(pArrayBase + extraMemoryInBytes);
		for (usize i = 0; i < count; ++i) {
			new(&pArrayData[i]) T(std::forward<ConstructorArgs>(args)...);
		}

		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pArrayData != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog<T>(pArrayData, MemoryOp::NewArray); };

		return reinterpret_cast<T*>(pArrayData);
	}

	// Calls de destructor of each element in the array and frees the memory
	// Must only be called with arrays allocated by CKE::NewArray()
	//
	// Example:
	//     u32* arr = CKE::NewArray(10, 0);
	//     CKE::DeleteArray(arr);
	template <typename T, typename... ConstructorArgs>
	void DeleteArray(T*& pArray) {
		if constexpr (CKE_MEMORY_ASSERT_ENABLED) { CKE_ASSERT(pArray != nullptr); }
		if constexpr (CKE_MEMORY_LOG) { MemoryLog<T>(pArray, MemoryOp::DeleteArray); };

		usize constexpr alignment = alignof(T);
		usize constexpr paddingForArrayCount = sizeof(usize);
		usize constexpr extraMemoryInBytes = std::max(alignment, paddingForArrayCount);

		u8*         pBaseAddress = reinterpret_cast<u8*>(pArray) - extraMemoryInBytes;
		usize const elemCount = *pBaseAddress;

		for (usize i = 0; i < elemCount; ++i) {
			pArray[i].~T();
		}

		Free(reinterpret_cast<void*&>(pBaseAddress));
		pArray = nullptr;
	}

	// Smart Pointers
	//-----------------------------------------------------------------------------

	// Owns and manages the lifetime of a pointer
	//
	// Example:
	//     UPtr<Object> o = MakeUPtr<Object>("Constructor Arg", 23, 53.3f);
	template <typename T>
	using UPtr = std::unique_ptr<T>;

	// Reference counted smart pointer
	//
	// Example:
	//     Rc<Object> o = MakeRc<Object>("Constructor Arg", 23, 53.3f);
	template <typename T>
	using Rc = std::shared_ptr<T>;

	// Creates a UPtr (Unique Pointer) of the given type, using the provided construction arguments
	template <typename T, typename... ConstructorArgs>
	[[nodiscard]] CKE_FORCE_INLINE UPtr<T> MakeUPtr(ConstructorArgs&&... args) {
		return UPtr<T>(CKE::New<T>(std::forward<ConstructorArgs>(args)...));
	}

	// Creates a Rc (Reference Counted Smart Pointer) of the given type, using the provided construction arguments
	template <typename T, typename... ConstructorArgs>
	[[nodiscard]] CKE_FORCE_INLINE UPtr<T> MakeRc(ConstructorArgs&&... args) {
		return std::make_shared<T>(std::forward<ConstructorArgs>(args)...);
	}

	//-----------------------------------------------------------------------------
}
