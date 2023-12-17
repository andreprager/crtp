#pragma once

#include <crtp/storage/detail/concept.decl.hpp>

#include <memory>

namespace crtp::storage
{
/// @brief:
template<typename TStorage>
class OnHeap
{
public:
	using storage_t = TStorage;
	using concept_t = storage_t::concept_t;
	using concept_ptr_t = std::unique_ptr<concept_t>;

	template<typename T>
	OnHeap( T value );
	OnHeap( OnHeap const& src );
	OnHeap( OnHeap&& src ) noexcept;
	~OnHeap();
	OnHeap& operator=( OnHeap const& src );
	OnHeap& operator=( OnHeap&& src ) noexcept;

	OnHeap& swap( OnHeap& src );
	/// @pre: src not empty
	void swap_data( concept_ptr_t& src );

	concept_t const* memory() const;
	concept_t*       memory();

private:
	concept_ptr_t m_data;
};

template<typename TStorage>
struct IsStorage<OnHeap<TStorage>> : std::true_type
{};

template<typename TStorage>
void swap( OnHeap<TStorage>& lsh, OnHeap<TStorage>& rsh );

} // namespace crtp::storage
