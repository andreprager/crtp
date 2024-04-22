#pragma once

#include "crtp/storage/swap/swap.decl.hpp"

#include <crtp/storage/hybrid/hybrid.impl.hpp>
#include <crtp/storage/on_heap/on_heap.impl.hpp>
#include <crtp/storage/on_stack/on_stack.impl.hpp>

#include <stdexcept>

namespace crtp::storage
{
template<traits::Policy P0, traits::Policy P1>
inline void swap_unsafe(P0& lsh, P1& rsh)
{
	if (!swap(lsh, rsh))
	{
		throw std::invalid_argument( "Swap for storage.Policy failed." );
	}
}

// OnHeap - OnStack

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline bool swap( OnHeap<TBuilder>& lsh, OnStack<TBuilder, Size, Alignment>& rsh ) noexcept
{
	if ( Size < lsh.memory()->size() )
	{
		return false;
	}
	auto tmp = rsh.memory()->extract();
	lsh.swap_data( tmp );
	rsh.replace( std::move( *tmp ) );
	return true;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline bool swap( OnStack<TBuilder, Size, Alignment>& lsh, OnHeap<TBuilder>& rsh ) noexcept
{
	return swap( rsh, lsh );
}

// Hybrid - OnStack

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
inline bool swap( Hybrid<TBuilder, Size, Alignment>& lsh, OnStack<TBuilder, Size2, Alignment>& rsh ) noexcept
{
	if ( Size2 < lsh.memory()->size() )
	{
		return false;
	}
	lsh.swap_data( rsh.buffer() );
	return true;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
inline bool swap( OnStack<TBuilder, Size2, Alignment>& lsh, Hybrid<TBuilder, Size, Alignment>& rsh ) noexcept
{
	return swap( rsh, lsh );
}

// Hybrid - OnHeap

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline bool swap( Hybrid<TBuilder, Size, Alignment>& lsh, OnHeap<TBuilder>& rsh ) noexcept
{
	auto tmp = lsh.memory()->extract();
	rsh.swap_data( tmp );
	lsh.replace( std::move( *tmp ) );
	return true;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline bool swap( OnHeap<TBuilder>& lsh, Hybrid<TBuilder, Size, Alignment>& rsh ) noexcept
{
	return swap( rsh, lsh );
}

} // namespace crtp::storage
