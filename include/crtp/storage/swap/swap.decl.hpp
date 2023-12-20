#pragma once

#include <crtp/storage/hybrid/hybrid.decl.hpp>
#include <crtp/storage/on_heap/on_heap.decl.hpp>
#include <crtp/storage/on_stack/on_stack.decl.hpp>

namespace crtp::storage
{
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
void swap( OnHeap<TStorage>& lsh, OnStack<TStorage, TSize, TAlignment>& rsh );
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
void swap( OnStack<TStorage, TSize, TAlignment>& lsh, OnHeap<TStorage>& rsh );
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
bool try_swap( OnHeap<TStorage>& lsh, OnStack<TStorage, TSize, TAlignment>& rsh );
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
bool try_swap( OnStack<TStorage, TSize, TAlignment>& lsh, OnHeap<TStorage>& rsh );

/// todo: swap for OnHeap/OnStack and Hybrid


} // namespace crtp::storage
