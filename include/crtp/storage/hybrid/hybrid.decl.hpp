#pragma once

#include <crtp/storage/builder/builder.decl.hpp>
#include <crtp/storage/traits/traits.hpp>

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
template<IBuilder TBuilder, std::size_t Size = 128, std::size_t Alignment = 16>
class Hybrid
{
public:
	using builder_t = TBuilder;
	using concept_t = builder_t::concept_t;
	using clone_t   = builder_t::clone_t;
	using buffer_t  = std::array<std::byte, Size>;
	template<std::size_t Size2>
	using buffer_other_t = std::array<std::byte, Size2>;
	template<std::size_t Size2>
	using HybridSrc = Hybrid<TBuilder, Size2, Alignment>;

	Hybrid( traits::NotPolicy auto value );

	Hybrid( Hybrid const& src );
	Hybrid( Hybrid&& src ) noexcept;

	Hybrid( traits::PolicyAssignOther<Hybrid<TBuilder, Size, Alignment>> auto const& src );
	Hybrid( traits::PolicyAssignOther<Hybrid<TBuilder, Size, Alignment>> auto&& src );

	~Hybrid();

	Hybrid& operator=( Hybrid const& src );
	Hybrid& operator=( Hybrid&& src ) noexcept;

	Hybrid& operator=( traits::PolicyAssignOther<Hybrid<TBuilder, Size, Alignment>> auto const& src ) noexcept;
	Hybrid& operator=( traits::PolicyAssignOther<Hybrid<TBuilder, Size, Alignment>> auto&& src ) noexcept;

	concept_t const* memory() const;
	concept_t*       memory();

	/// @brief: Copy construct @src into m_data, via clone. Previous buffer content is destroyed if any.
	Hybrid& replace( concept_t const& src );
	/// @brief: Move construct @src into m_data, via extract. Previous buffer content is destroyed if any.
	Hybrid& replace( concept_t&& src ) noexcept;
	/// @brief: Move construct @src into m_data into pointer if Size < src->size() otherwise into buffer.
	///         Previous buffer content is destroyed if any.
	Hybrid& replace( clone_t src ) noexcept;

	Hybrid& swap( Hybrid& src ) noexcept;
	template<std::size_t Size2>
	Hybrid& swap( HybridSrc<Size2>& src, std::enable_if_t<Size != Size2, int> = 0 ) noexcept;
	/// @pre: src must contain an inplace constructed concept_t.
	template<std::size_t Size2>
	bool swap_data( buffer_other_t<Size2>& src ) noexcept;

private:
	using data_t = std::variant<buffer_t, std::unique_ptr<concept_t>>;

	/// @brief: Copy construct @src into m_data, via clone. Previous buffer content is not destroyed if any.
	///         Selection based on src.size().
	/// @pre: m_data must not be constructed yet.
	Hybrid& construct( concept_t const& src );
	/// @brief: Copy construct @src into m_data, via clone. Previous buffer content is not destroyed if any.
	///         Selection based on src.m_data.index().
	/// @pre: m_data must not be constructed yet.
	Hybrid& construct( Hybrid const& src );
	/// @brief: Move construct @src into m_data, via extract. Previous buffer content is not destroyed if any.
	///         Selection based on src.size().
	/// @pre: m_data must not be constructed yet.
	Hybrid& construct( concept_t&& src ) noexcept;
	/// @brief: Move construct @src into m_data, via extract. Previous buffer content is not destroyed if any.
	///         Selection based on src.m_data.index().
	/// @pre: m_data must not be constructed yet.
	Hybrid& construct( Hybrid&& src ) noexcept;
	/// @brief: Move construct @src into m_data. via move (Size < src.size()) or move construct. Previous buffer content
	/// is not destroyed if any.
	/// @pre: m_data must not be constructed yet.
	Hybrid& construct( clone_t src ) noexcept;

	/// @pre: 0 == m_data.index()
	concept_t const* buffer() const;
	/// @pre: 0 == m_data.index()
	concept_t* buffer();
	/// @post: Destroy buffer if needed.
	Hybrid& destroy() noexcept;
	/// @pre: 0 == m_data.index()
	/// @post: buffer is destroyed if any
	Hybrid& destroy_buffer() noexcept;
	/// @brief: Swap buffer (@this) and unique_ptr (@src) for same size.
	/// @pre: 0 == m_data.index()
	/// @pre: 1 == src.m_data.index()
	Hybrid& swap_buffer_pointer( Hybrid& src ) noexcept;
	/// @brief: Swap @this with buffer (@src) with respect to target buffer sizes.
	/// @pre: 0 == src.m_data.index()
	template<std::size_t Size2>
	Hybrid& swap_buffer( HybridSrc<Size2>& src, std::enable_if_t<Size != Size2, int> = 0 ) noexcept;
	/// @brief: Swap @this with unique_ptr (@src) with respect to target buffer sizes.
	/// @pre: 1 == src.m_data.index()
	template<std::size_t Size2>
	Hybrid& swap_pointer( HybridSrc<Size2>& src, std::enable_if_t<Size != Size2, int> = 0 ) noexcept;

	alignas( Alignment ) data_t m_data;

#if defined( _MSC_VER )
	[[msvc::no_unique_address]] builder_t m_builder{};
#else
	[[no_unique_address]] builder_t m_builder{};
#endif

	template<IBuilder TBuilder2, std::size_t Size2, std::size_t Alignment2>
	friend class Hybrid;
};

template<typename TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
void swap( Hybrid<TBuilder, Size, Alignment>& lsh, Hybrid<TBuilder, Size2, Alignment>& rsh ) noexcept;

template<typename TBuilder, std::size_t Size, std::size_t Alignment>
struct BuilderConfig<Hybrid<TBuilder, Size, Alignment>>
{
	static constexpr std::size_t size  = Size;
	static constexpr std::size_t align = Alignment;
};

} // namespace crtp::storage
