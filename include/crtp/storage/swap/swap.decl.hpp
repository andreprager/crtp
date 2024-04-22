#pragma once

#include <crtp/storage/hybrid/hybrid.decl.hpp>
#include <crtp/storage/on_heap/on_heap.decl.hpp>
#include <crtp/storage/on_stack/on_stack.decl.hpp>
#include <crtp/storage/traits/traits.hpp>

namespace crtp::storage
{
template<traits::Policy P0, traits::Policy P1>
void swap_unsafe( P0& lsh, P1& rsh );

// OnHeap - OnStack

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
bool swap( OnHeap<TBuilder>& lsh, OnStack<TBuilder, Size, Alignment>& rsh ) noexcept;
template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
bool swap( OnStack<TBuilder, Size, Alignment>& lsh, OnHeap<TBuilder>& rsh ) noexcept;

// Hybrid - OnStack

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
bool swap( Hybrid<TBuilder, Size, Alignment>& lsh, OnStack<TBuilder, Size2, Alignment>& rsh ) noexcept;
template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
bool swap( OnStack<TBuilder, Size2, Alignment>& lsh, Hybrid<TBuilder, Size, Alignment>& rsh ) noexcept;

// Hybrid - OnHeap

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
bool swap( Hybrid<TBuilder, Size, Alignment>& lsh, OnHeap<TBuilder>& rsh ) noexcept;
template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
bool swap( OnHeap<TBuilder>& lsh, Hybrid<TBuilder, Size, Alignment>& rsh ) noexcept;

/// todo: swap for OnHeap/OnStack and Hybrid

} // namespace crtp::storage
