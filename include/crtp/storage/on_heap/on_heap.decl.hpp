#pragma once

#include <crtp/storage/concept/concept.decl.hpp>

#include <memory>

namespace crtp::storage
{
/// @brief:     Straigh forward smart pointer type erasure.
/// @tparam:    TBuilder builder strategy, see crtp/storage/builder module for required interface.
/// @invariant: Always contains valid concept_t inside @m_data.
template<typename TBuilder>
class OnHeap
{
public:
	using builder_t = TBuilder;
	using concept_t = builder_t::concept_t;
	using concept_ptr_t = builder_t::concept_ptr_t;

	template<typename T>
	OnHeap( T value );
	OnHeap( OnHeap const& src );
	OnHeap( OnHeap&& src ) noexcept;
	~OnHeap();
	OnHeap& operator=( OnHeap const& src );
	OnHeap& operator=( OnHeap&& src ) noexcept;

	OnHeap& swap( OnHeap& src );
	/// @brief: Swap with externally created non-empty concept.
	/// @pre:   src not empty
	void swap_data( concept_ptr_t& src );

	concept_t const* memory() const;
	concept_t*       memory();

private:
	concept_ptr_t m_data;
};

template<typename TBuilder>
struct IsStorage<OnHeap<TBuilder>> : std::true_type
{};

template<typename TBuilder>
void swap( OnHeap<TBuilder>& lsh, OnHeap<TBuilder>& rsh );

} // namespace crtp::storage
