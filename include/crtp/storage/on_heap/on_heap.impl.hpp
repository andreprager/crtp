#pragma once

#include "crtp/storage/on_heap/on_heap.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

namespace crtp::storage
{
template<IBuilder TBuilder>
inline OnHeap<TBuilder>::OnHeap( traits::NotPolicy auto value )
  : m_data{ [ & ]() {
	  using T = std::decay_t<decltype( value )>;
	  static_assert( IBuilderHeap<builder_t, T>, "OnHeap requires IBuilderHeap<TBuilder, T>." );
	  return builder_t::build( std::move( value ) );
	}() }
{}

template<IBuilder TBuilder>
inline OnHeap<TBuilder>::OnHeap( OnHeap const& src )
{
	src.m_data->clone( m_data );
}

template<IBuilder TBuilder>
inline OnHeap<TBuilder>::OnHeap( OnHeap&& src ) noexcept
{
	src.m_data->extract( m_data );
}

template<IBuilder TBuilder>
inline OnHeap<TBuilder>::~OnHeap()
{
	m_data.reset();
}

template<IBuilder TBuilder>
inline auto OnHeap<TBuilder>::operator=( OnHeap const& src ) -> OnHeap&
{
	if ( this == &src )
	{
		return *this;
	}
	src.m_data->clone( m_data );
	return *this;
}

template<IBuilder TBuilder>
inline auto OnHeap<TBuilder>::operator=( OnHeap&& src ) noexcept -> OnHeap&
{
	if ( this == &src )
	{
		return *this;
	}
	src.m_data->extract( m_data );
	return *this;
}

template<IBuilder TBuilder>
inline auto OnHeap<TBuilder>::swap( OnHeap& src ) noexcept -> OnHeap&
{
	if ( this == &src )
	{
		return *this;
	}
	m_data.swap( src.m_data );
	return *this;
}

template<IBuilder TBuilder>
inline void OnHeap<TBuilder>::swap_data( clone_t& src ) noexcept
{
	m_data.swap( src );
}

template<IBuilder TBuilder>
inline auto OnHeap<TBuilder>::memory() const -> concept_t const*
{
	return m_data.get();
}

template<IBuilder TBuilder>
inline auto OnHeap<TBuilder>::memory() -> concept_t*
{
	return m_data.get();
}

template<typename TBuilder>
inline void swap( OnHeap<TBuilder>& lsh, OnHeap<TBuilder>& rsh ) noexcept
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
