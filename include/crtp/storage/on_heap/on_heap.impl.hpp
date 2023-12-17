#pragma once

#include "crtp/storage/on_heap/on_heap.decl.hpp"

#include "crtp/storage/detail/concept.impl.hpp"

namespace crtp::storage
{
/// OnHeap

template<typename TStorage>
template<typename T>
inline OnHeap<TStorage>::OnHeap( T value ) : m_data{ storage_t::build( std::move( value ) ) }
{}

template<typename TStorage>
inline OnHeap<TStorage>::OnHeap( OnHeap const& src ) : m_data{ src.m_data->clone() }
{}

template<typename TStorage>
inline OnHeap<TStorage>::OnHeap( OnHeap&& src ) noexcept : m_data{ src.m_data->extract() }
{}

template<typename TStorage>
inline OnHeap<TStorage>::~OnHeap()
{
	m_data.reset();
}

template<typename TStorage>
inline auto OnHeap<TStorage>::operator=( OnHeap const& src ) -> OnHeap&
{
	m_data = src.m_data->clone();
	return *this;
}

template<typename TStorage>
inline auto OnHeap<TStorage>::operator=( OnHeap&& src ) noexcept -> OnHeap&
{
	m_data = src.m_data->extract();
	return *this;
}

template<typename TStorage>
inline auto OnHeap<TStorage>::swap( OnHeap& src ) -> OnHeap&
{
	m_data.swap( src.m_data );
	return *this;
}

template<typename TStorage>
inline auto OnHeap<TStorage>::memory() const -> concept_t const*
{
	return m_data.get();
}

template<typename TStorage>
inline auto OnHeap<TStorage>::memory() -> concept_t*
{
	return m_data.get();
}

template<typename TStorage>
inline void swap( OnHeap<TStorage>& lsh, OnHeap<TStorage>& rsh )
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
