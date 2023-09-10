#pragma once

#include "ComponentIter.h"

namespace CKE {

	// Template iterator for multi-component queries
	template <typename Comp, typename... OtherComp>
	class TMultiComponentIter : public MultiComponentIter
	{
	public:
		explicit TMultiComponentIter(EntityDatabase* pEntityAdmin);

		// Range-for iterator
		inline TMultiComponentIter               begin();
		inline std::tuple<Comp*, OtherComp*...>& operator*();

	private:
		// Auxiliary
		template <size_t I = 0, typename... Ts>
		constexpr inline void PopulateVectorWithComponentIDs(Vector<ComponentTypeID>& vec);
		template <size_t I = 0, typename... Ts>
		constexpr inline void PopulateTupleWithComponents(std::tuple<Ts...>& tuple);

	private:
		// Cached component tuple that the iterator returns
		std::tuple<Comp*, OtherComp*...> m_CompTuple;
	};

	//-----------------------------------------------------------------------------

	// Template iterator for single-component queries
	template <typename T>
	class TComponentIterator : public ComponentIter
	{
	public:
		explicit TComponentIterator(EntityDatabase* pEntityAdmin);

		// Range-for iterator
		inline TComponentIterator begin();
		inline T*                 operator*();
	};
}

// Template Implementations
//-----------------------------------------------------------------------------

namespace CKE {
	template <typename Comp, typename... Other>
	template <size_t I, typename... Ts>
	constexpr void TMultiComponentIter<Comp, Other...>::PopulateVectorWithComponentIDs(Vector<ComponentTypeID>& vec) {
		if constexpr (I == sizeof...(Ts)) { return; }
		else {
			vec.emplace_back(ComponentStaticTypeID<std::tuple_element_t<I, std::tuple<Ts...>>>::s_CompID);
			PopulateVectorWithComponentIDs<I + 1, Ts...>(vec);
		}
	}

	template <typename Comp, typename... Other>
	template <size_t I, typename... Ts>
	constexpr void TMultiComponentIter<Comp, Other...>::PopulateTupleWithComponents(std::tuple<Ts...>& tuple) {
		if constexpr (I == sizeof...(Ts)) { return; }
		else {
			std::get<I>(tuple) = (std::tuple_element_t<I, std::tuple<Ts...>>)m_pCurrCompArrays.at(I)->
					GetCompAtIndex(m_CurrRowInArch);
			PopulateTupleWithComponents<I + 1>(tuple);
		}
	}

	template <typename Comp, typename... Other>
	TMultiComponentIter<Comp, Other...>::TMultiComponentIter(EntityDatabase* pEntityAdmin) {
		// Convert template tuple into a vector with the component IDs
		Vector<ComponentTypeID> componentIDs;
		PopulateVectorWithComponentIDs<0, Comp, Other...>(componentIDs);
		Initialize(pEntityAdmin, componentIDs);
	}

	template <typename Comp, typename... Other>
	TMultiComponentIter<Comp, Other...> TMultiComponentIter<Comp, Other...>::begin() {
		BaseBeginIteratorSetup();
		return *this;
	}

	template <typename Comp, typename... Other>
	std::tuple<Comp*, Other*...>& TMultiComponentIter<Comp, Other...>::operator*() {
		PopulateTupleWithComponents(m_CompTuple);
		return m_CompTuple;
	}

	//-----------------------------------------------------------------------------

	template <typename T>
	TComponentIterator<T>::TComponentIterator(EntityDatabase* pEntityAdmin) {
		Initialize(pEntityAdmin, ComponentStaticTypeID<T>::s_CompID);
	}

	template <typename T>
	TComponentIterator<T> TComponentIterator<T>::begin() {
		SetupBegin();
		return *this;
	}

	template <typename T>
	T* TComponentIterator<T>::operator*() {
		return reinterpret_cast<T*>(ComponentIter::operator*());
	}
}
