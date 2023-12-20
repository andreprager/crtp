#pragma once

#include "crtp/storage/on_heap/on_heap.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

namespace crtp::storage
{
/// OnHeap

template<typename TBuilder>
template<typename T>
inline OnHeap<TBuilder>::OnHeap( T value ) : m_data{ builder_t::build( std::move( value ) ) }
{}

template<typename TBuilder>
inline OnHeap<TBuilder>::OnHeap( OnHeap const& src ) : m_data{ src.m_data->clone() }
{}

template<typename TBuilder>
inline OnHeap<TBuilder>::OnHeap( OnHeap&& src ) noexcept : m_data{ src.m_data->extract() }
{}

template<typename TBuilder>
inline OnHeap<TBuilder>::~OnHeap()
{
	m_data.reset();
}

template<typename TBuilder>
inline auto OnHeap<TBuilder>::operator=( OnHeap const& src ) -> OnHeap&
{
	m_data = src.m_data->clone();
	return *this;
}

template<typename TBuilder>
inline auto OnHeap<TBuilder>::operator=( OnHeap&& src ) noexcept -> OnHeap&
{
	m_data = src.m_data->extract();
	return *this;
}

template<typename TBuilder>
inline auto OnHeap<TBuilder>::swap( OnHeap& src ) -> OnHeap&
{
	m_data.swap( src.m_data );
	return *this;
}

template<typename TBuilder>
inline void OnHeap<TBuilder>::swap_data( concept_ptr_t& src )
{
	m_data.swap(src);
}

template<typename TBuilder>
inline auto OnHeap<TBuilder>::memory() const -> concept_t const*
{
	return m_data.get();
}

template<typename TBuilder>
inline auto OnHeap<TBuilder>::memory() -> concept_t*
{
	return m_data.get();
}

template<typename TBuilder>
inline void swap( OnHeap<TBuilder>& lsh, OnHeap<TBuilder>& rsh )
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
