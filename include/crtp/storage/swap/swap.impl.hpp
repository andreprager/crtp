#pragma once

#include "crtp/storage/swap/swap.decl.hpp"

#include <crtp/storage/hybrid/hybrid.impl.hpp>
#include <crtp/storage/on_heap/on_heap.impl.hpp>
#include <crtp/storage/on_stack/on_stack.impl.hpp>

#include <stdexcept>

namespace crtp::storage
{
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void swap( OnHeap<TStorage>& lsh, OnStack<TStorage, TSize, TAlignment>& rsh )
{
	if ( TSize < lsh.memory()->size() )
	{
		throw std::invalid_argument( "Size of OnHeap<TStorage>& lsh too big." );
	}
	auto tmp = rsh.memory()->extract();
	lsh.swap_data( tmp );
	rsh.replace( std::move( *tmp ) );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void swap( OnStack<TStorage, TSize, TAlignment>& lsh, OnHeap<TStorage>& rsh )
{
	swap( rsh, lsh );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline bool try_swap( OnHeap<TStorage>& lsh, OnStack<TStorage, TSize, TAlignment>& rsh )
try
{
	swap( lsh, rsh );
	return true;
}
catch ( ... )
{
	return false;
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline bool try_swap( OnStack<TStorage, TSize, TAlignment>& lsh, OnHeap<TStorage>& rsh )
{
	return try_swap( rsh, lsh );
}

} // namespace crtp::storage
