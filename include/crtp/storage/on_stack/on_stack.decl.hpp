#pragma once

#include <crtp/storage/builder/builder.decl.hpp>

#include <array>
#include <cstddef>

namespace crtp::storage
{
/// @brief:     Small buffer optimized type erasure.
/// @tparam:    TBuilder builder strategy, see crtp/storage/builder module for required interface.
/// @pre:       No self references in concept implementations allowed.
/// @invariant: Always contains valid concept_t inside @m_data.
template<IBuilder TBuilder, std::size_t TSize = 128, std::size_t TAlignment = 16>
class OnStack
{
public:
	using builder_t = TBuilder;
	using concept_t = builder_t::concept_t;
	using buffer_t  = std::array<std::byte, TSize>;

	template<typename T>
	OnStack( T value );
	OnStack( OnStack const& src );
	OnStack( OnStack&& src ) noexcept;
	OnStack& operator=( OnStack const& src );
	OnStack& operator=( OnStack&& src ) noexcept;
	~OnStack();

	OnStack& swap( OnStack& src );
	OnStack& replace( concept_t const& src );
	OnStack& replace( concept_t&& src );

	concept_t const* memory() const;
	concept_t*       memory();

	static void swap_buffer( concept_t* lsh, concept_t* rsh );

private:
	/// @pre: memory() must yield a properly constructed concept_t.
	void destroy();

	alignas( TAlignment ) buffer_t m_data;
};

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
void swap( OnStack<TBuilder, TSize, TAlignment>& lsh, OnStack<TBuilder, TSize, TAlignment>& rsh );

} // namespace crtp::storage
