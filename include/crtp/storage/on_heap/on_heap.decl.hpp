#pragma once

#include <crtp/storage/builder/builder.decl.hpp>

#include <memory>

namespace crtp::storage
{
/// @brief:     Straigh forward smart pointer type erasure.
/// @tparam:    TBuilder builder strategy, see crtp/storage/builder module for required interface.
/// @invariant: Always contains valid concept_t inside @m_data.
template<IBuilder TBuilder>
class OnHeap
{
public:
	using builder_t     = TBuilder;
	using concept_t     = builder_t::concept_t;
	using clone_t   = builder_t::clone_t;

	OnHeap( traits::NotPolicy auto value );
	OnHeap( OnHeap const& src );
	OnHeap( OnHeap&& src ) noexcept;
	~OnHeap();
	OnHeap& operator=( OnHeap const& src );
	OnHeap& operator=( OnHeap&& src ) noexcept;

	OnHeap& swap( OnHeap& src ) noexcept;
	/// @brief: Swap with externally created non-empty concept.
	/// @pre:   src not empty
	void swap_data( clone_t& src ) noexcept;

	concept_t const* memory() const;
	concept_t*       memory();

private:
	clone_t m_data{};
};

template<typename TBuilder>
void swap( OnHeap<TBuilder>& lsh, OnHeap<TBuilder>& rsh ) noexcept;

} // namespace crtp::storage
