#pragma once

#include <crtp/storage/builder/builder.decl.hpp>
#include <crtp/storage/traits/traits.hpp>

#include <array>
#include <cstddef>

namespace crtp::storage
{
/// @brief:     Small buffer optimized type erasure.
/// @tparam:    TBuilder builder strategy, see crtp/storage/builder module for required interface.
/// @pre:       No self references in concept implementations allowed.
/// @invariant: Always contains valid concept_t inside @m_data.
template<IBuilder TBuilder, std::size_t Size = 128, std::size_t Alignment = 16>
class OnStack
{
public:
	using builder_t = TBuilder;
	using concept_t = builder_t::concept_t;
	using clone_t   = builder_t::clone_t;
	using buffer_t  = std::array<std::byte, Size>;
	template<std::size_t Size2>
	using buffer_other_t = std::array<std::byte, Size2>;
	template<std::size_t Size2>
	using OnStackSrc = OnStack<builder_t, Size2, Alignment>;

	OnStack( traits::NotPolicy auto value );

	OnStack( OnStack const& src );
	OnStack( OnStack&& src ) noexcept;

	OnStack( traits::PolicyAssignOther<OnStack<TBuilder, Size, Alignment>> auto const& src );
	OnStack( traits::PolicyAssignOther<OnStack<TBuilder, Size, Alignment>> auto&& src );

	~OnStack();

	OnStack& operator=( OnStack const& src );
	OnStack& operator=( OnStack&& src ) noexcept;

	OnStack& operator=( traits::PolicyAssignOther<OnStack<TBuilder, Size, Alignment>> auto const& src );
	OnStack& operator=( traits::PolicyAssignOther<OnStack<TBuilder, Size, Alignment>> auto&& src );

	/// @brief:
	buffer_t&       buffer();
	buffer_t const& buffer() const;

	concept_t const* memory() const;
	concept_t*       memory();

	/// @brief: Copy construct @src into buffer m_data, via clone.
	/// @pre:   Size of @src <= @Size
	OnStack& replace( concept_t const& src );
	/// @brief: Move construct @src into buffer m_data, via extract.
	/// @pre:   Size of @src <= @Size
	OnStack& replace( concept_t&& src );

	/// @brief:  Try to swap underlying concepts of two buffers.
	/// @pre:    @src must contain an inplace constructed concept_t.
	/// @return: True if successful swapped (sizes compatible), otherwise false (only for Size != Size2).
	template<std::size_t Size2>
	bool swap_data( buffer_other_t<Size2>& src ) noexcept;

	/// @brief:  Try to swap underlying concepts of two buffers.
	/// @return: True if successful swapped (sizes compatible), otherwise false (only for Size != Size2).
	template<std::size_t Size2>
	bool swap( OnStackSrc<Size2>& src ) noexcept;

	/// @brief:  Try to swap underlying concepts of two buffers.
	template<std::size_t Size2>
	OnStack& swap_unsafe( OnStackSrc<Size2>& src );

private:
	/// @pre: memory() must yield a properly constructed concept_t.
	OnStack& destroy();

	alignas( Alignment ) buffer_t m_data;
#if defined( _MSC_VER )
	[[msvc::no_unique_address]] builder_t m_builder{};
#else
	[[no_unique_address]] builder_t m_builder{};
#endif

	template<IBuilder TBuilder2, std::size_t Size2, std::size_t Alignment2>
	friend class OnStack;
};

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
auto swap( OnStack<TBuilder, Size, Alignment>& lsh, OnStack<TBuilder, Size2, Alignment>& rsh ) noexcept;

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
struct BuilderConfig<OnStack<TBuilder, Size, Alignment>>
{
	static constexpr std::size_t size  = Size;
	static constexpr std::size_t align = Alignment;
};

} // namespace crtp::storage
