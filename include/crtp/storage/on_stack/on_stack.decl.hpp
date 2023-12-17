#pragma once

#include <crtp/storage/detail/concept.decl.hpp>

#include <array>
#include <concepts>
#include <cstddef>
#include <variant>

namespace crtp::storage
{
/// @brief:     Small buffer optimized type erasure.
/// @pre:       No self references in types allowed
/// @tparam:    TStorage Some storage value
/// @invariant: Always contains valid concept_t inside @m_data.
template<typename TStorage, std::size_t TSize = 128, std::size_t TAlignment = 16>
class OnStack
{
public:
	using storage_t   = TStorage;
	using concept_t = storage_t::concept_t;
	using buffer_t  = std::array<std::byte, TSize>;

	template<typename T>
	OnStack( T value );
	OnStack( OnStack const& src );
	OnStack( OnStack&& src ) noexcept;
	OnStack& operator=( OnStack const& src );
	OnStack& operator=( OnStack&& src ) noexcept;
	~OnStack();

	OnStack& swap( OnStack& src );

	concept_t const* memory() const;
	concept_t*       memory();

	static void swap_buffer( concept_t* lsh, concept_t* rsh );

private:
	/// @pre: memory() must yield a properly constructed concept_t.
	void destroy();

	alignas( TAlignment ) buffer_t m_data;
};

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
struct IsStorage<OnStack<TStorage, TSize, TAlignment>> : std::true_type
{};

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
void swap( OnStack<TStorage, TSize, TAlignment>& lsh, OnStack<TStorage, TSize, TAlignment>& rsh );

} // namespace crtp::storage
