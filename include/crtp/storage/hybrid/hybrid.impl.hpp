#pragma once

#include "crtp/storage/hybrid/hybrid.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"
#include "crtp/storage/on_stack/on_stack.impl.hpp"

#include <cassert>

namespace crtp::storage
{
template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<typename T>
inline Hybrid<TBuilder, Size, Alignment>::Hybrid( T value )
{
	if constexpr ( m_builder.template size<T>() <= Size )
	{
		static_assert( IBuilderInplace<builder_t, Size, Alignment, T>, "Hybrid requires IBuilderInplace<TBuilder, T>." );
		m_builder.template inplace<Size, Alignment>( buffer(), std::move( value ) );
	}
	else
	{
		static_assert( IBuilderHeap<builder_t, T>, "Hybrid requires IBuilderHeap<TBuilder, T>." );
		m_data = m_builder.build( std::move( value ) );
	}
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline Hybrid<TBuilder, Size, Alignment>::Hybrid( Hybrid const& src )
{
	construct( src );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline Hybrid<TBuilder, Size, Alignment>::Hybrid( Hybrid&& src ) noexcept
{
	construct( std::move( src ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline Hybrid<TBuilder, Size, Alignment>::Hybrid(
    traits::PolicyAssignOther<Hybrid<TBuilder, Size, Alignment>> auto src )
{
	construct( std::move( *src.memory() ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline Hybrid<TBuilder, Size, Alignment>::~Hybrid()
{
	destroy();
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::operator=( Hybrid const& src ) -> Hybrid&
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );

	return destroy().construct( src );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::operator=( Hybrid&& src ) noexcept -> Hybrid&
{
	return destroy().construct( std::move( src ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::operator=(
    traits::PolicyAssignOther<Hybrid<TBuilder, Size, Alignment>> auto src ) noexcept -> Hybrid&
{
	return replace( std::move( *src.memory() ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::memory() const -> concept_t const*
{
	return ( 0 == m_data.index() ) ? buffer() : std::get<1>( m_data ).get();
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::memory() -> concept_t*
{
	return ( 0 == m_data.index() ) ? buffer() : std::get<1>( m_data ).get();
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::replace( concept_t const& src ) -> Hybrid&
{
	return destroy().construct( src );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::replace( concept_t&& src ) noexcept -> Hybrid&
{
	return destroy().construct( std::move( src ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::replace( clone_t src ) noexcept -> Hybrid&
{
	return destroy().construct( std::move( src ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline bool Hybrid<TBuilder, Size, Alignment>::swap_data( buffer_other_t<Size2>& src ) noexcept
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );

	if ( Size2 < memory()->size() )
	{
		return false;
	}
	alignas( Alignment ) buffer_other_t<Size2> data;

	auto tmp = m_builder.memory( data );
	auto rsh = m_builder.memory( src );
	rsh->extract( tmp );
	m_builder.destroy( src );
	memory()->extract( rsh );
	replace( std::move( *tmp ) );
	return true;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::construct( Hybrid const& src ) -> Hybrid&
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );

	if ( 0 == src.m_data.index() )
	{
		m_data.emplace<0>();
		src.memory()->clone( buffer() );
	}
	else
	{
		m_data.emplace<1>();
		std::get<1>( src.m_data )->clone( std::get<1>( m_data ) );
	}
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::construct( concept_t const& src ) -> Hybrid&
{
	if ( src.size() <= Size )
	{
		m_data.emplace<0>();
		src.clone( buffer() );
	}
	else
	{
		m_data.emplace<1>();
		src.clone( std::get<1>( m_data ) );
	}
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::construct( Hybrid&& src ) noexcept -> Hybrid&
{
	if ( 0 == src.m_data.index() )
	{
		m_data.emplace<0>();
		src.memory()->extract( buffer() );
	}
	else
	{
		m_data.emplace<1>();
		std::get<1>( src.m_data )->extract( std::get<1>( m_data ) );
	}
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::construct( concept_t&& src ) noexcept -> Hybrid&
{
	if ( src.size() <= Size )
	{
		m_data.emplace<0>();
		src.extract( buffer() );
	}
	else
	{
		m_data.emplace<1>();
		src.extract( std::get<1>( m_data ) );
	}
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::construct( clone_t src ) noexcept -> Hybrid&
{
	if ( src->size() <= Size )
	{
		m_data.emplace<0>();
		src->extract( buffer() );
	}
	else
	{
		m_data = std::move( src );
	}
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::buffer() const -> concept_t const*
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );
	return m_builder.memory( std::get<0>( m_data ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::buffer() -> concept_t*
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );
	return m_builder.memory( std::get<0>( m_data ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::destroy() noexcept -> Hybrid&
{
	return ( 0 == m_data.index() ) ? destroy_buffer() : *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::destroy_buffer() noexcept -> Hybrid&
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );
	m_builder.destroy( std::get<0>( m_data ) );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::swap_buffer_pointer( Hybrid& src ) noexcept -> Hybrid&
{
	// save @src unique_ptr
	auto tmp = std::move( std::get<1>( src.m_data ) );
	// move this buffer concept to @src
	src.m_data.emplace<0>();
	buffer()->extract( src.buffer() );
	destroy_buffer();
	// move saved @src unique_ptr to this
	m_data = std::move( tmp );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline auto Hybrid<TBuilder, Size, Alignment>::swap_buffer( HybridSrc<Size2>& src,
                                                            std::enable_if_t<Size != Size2, int> ) noexcept -> Hybrid&
{
	static_assert( IBuilderStack<builder_t, Size>, "Hybrid requires IBuilderStack<TBuilder, Size>." );

	// save @src buffer
	alignas( Alignment ) buffer_other_t<Size2> data;

	auto tmp = m_builder.memory( data );
	auto rsh = m_builder.memory( std::get<0>( src.m_data ) );
	rsh->extract( tmp );
	// move this buffer to @src
	src.replace( std::move( *memory() ) );
	// move saved @src buffer to this
	replace( std::move( *tmp ) );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline auto Hybrid<TBuilder, Size, Alignment>::swap_pointer( HybridSrc<Size2>& src,
                                                             std::enable_if_t<Size != Size2, int> ) noexcept -> Hybrid&
{
	// check if pointer fits into others buffer variant
	auto tmp = std::move( std::get<1>( src.m_data ) );
	// move this buffer concept to @src
	src.replace( std::move( *memory() ) );
	// move saved @src unique_ptr to this
	replace( std::move( tmp ) );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto Hybrid<TBuilder, Size, Alignment>::swap( Hybrid& src ) noexcept -> Hybrid&
{
	if ( 0 == m_data.index() )
	{
		if ( 0 == src.m_data.index() )
		{
			swap_data( std::get<0>( src.m_data ) );
			return *this;
		}
		return swap_buffer_pointer( src );
	}
	if ( 0 == src.m_data.index() )
	{
		src.swap_buffer_pointer( *this );
		return *this;
	}
	std::get<1>( m_data ).swap( std::get<1>( src.m_data ) );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline auto
    Hybrid<TBuilder, Size, Alignment>::swap( HybridSrc<Size2>& src, std::enable_if_t<Size != Size2, int> ) noexcept
    -> Hybrid&
{
	return ( 0 == src.m_data.index() ) ? swap_buffer( src ) : swap_pointer( src );
}

template<typename TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
inline void swap( Hybrid<TBuilder, Size, Alignment>& lsh, Hybrid<TBuilder, Size2, Alignment>& rsh ) noexcept
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
