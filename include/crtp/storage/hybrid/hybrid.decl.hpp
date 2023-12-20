#pragma once

#include <crtp/storage/concept/concept.decl.hpp>

#include <array>
#include <cstddef>
#include <variant>

namespace crtp::storage
{
/// @brief:     Hybrid type erasure with small buffer optimization for types fitting into buffer,
///             otherwise fallback to smart pointer to concept.
/// @tparam:    TBuilder builder strategy, see crtp/storage/builder module for required interface.
/// @pre:       No self references in concept implementations allowed.
/// @invariant: Always contains valid concept_t inside @m_data.
template<typename TBuilder, std::size_t TSize = 128, std::size_t TAlignment = 16>
class Hybrid
{
public:
	using builder_t = TBuilder;
	using concept_t = builder_t::concept_t;
	using buffer_t  = std::array<std::byte, TSize>;

	template<typename T>
	Hybrid( T value );
	Hybrid( Hybrid const& src );
	Hybrid( Hybrid&& src ) noexcept;
	Hybrid& operator=( Hybrid const& src );
	Hybrid& operator=( Hybrid&& src ) noexcept;
	~Hybrid();

	Hybrid& swap( Hybrid& src );

	concept_t const* memory() const;
	concept_t*       memory();

private:
	void destroy();
	/// @pre: 0 == m_data.index()
	void destroy_buffer();
	/// @pre: 0 == m_data.index()
	concept_t const* buffer() const;
	/// @pre: 0 == m_data.index()
	concept_t* buffer();
	/// @pre: @this contains buffer variant in m_data.
	/// @pre: @src contains unique_ptr variant in m_data.
	void swap_buffer_pointer( Hybrid& src );

	alignas( TAlignment ) std::variant<buffer_t, std::unique_ptr<concept_t>> m_data;
};

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
struct IsStorage<Hybrid<TBuilder, TSize, TAlignment>> : std::true_type
{};

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
void swap( Hybrid<TBuilder, TSize, TAlignment>& lsh, Hybrid<TBuilder, TSize, TAlignment>& rsh );

} // namespace crtp::storage
