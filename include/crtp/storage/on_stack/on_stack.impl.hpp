#pragma once

#include "crtp/storage/on_stack/on_stack.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

#include <cassert>
#include <stdexcept>

namespace crtp::storage
{
template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<typename T>
inline OnStack<TBuilder, Size, Alignment>::OnStack( T value )
	: m_data{}
{
	static_assert( IBuilderInplace<builder_t, Size, Alignment, T>,
	               "OnStack requires IBuilderInplace<TBuilder, Size, Alignment, T>." );
	m_builder.template inplace<Size, Alignment>( memory(), std::move( value ) );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline OnStack<TBuilder, Size, Alignment>::OnStack( OnStack const& src ) : m_data{}
{
	src.memory()->clone( memory() );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline OnStack<TBuilder, Size, Alignment>::OnStack( OnStack&& src ) noexcept : m_data{}
{
	src.memory()->extract( memory() );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline OnStack<TBuilder, Size, Alignment>::OnStack(
    traits::PolicyAssignOther<OnStack<TBuilder, Size, Alignment>> auto src )
  : m_data{}
{
	if ( Size < src.memory()->size() )
	{
		throw std::length_error( "OnStack.extract: Buffer too small." );
	}
	src.memory()->extract( memory() );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline OnStack<TBuilder, Size, Alignment>::~OnStack()
{
	destroy();
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::operator=( OnStack const& src ) -> OnStack&
{
	destroy();
	src.memory()->clone( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::operator=( OnStack&& src ) noexcept -> OnStack&
{
	destroy();
	src.memory()->extract( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::operator=(
    traits::PolicyAssignOther<OnStack<TBuilder, Size, Alignment>> auto src ) -> OnStack&
{
	if ( Size < src.memory()->size() )
	{
		throw std::length_error( "OnStack.extract assign: Buffer too small." );
	}
	destroy();
	src.memory()->extract( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::destroy() -> OnStack&
{
	static_assert( IBuilderStack<builder_t, Size>, "OnStack requires IBuilderStack<TBuilder, Size>." );
	m_builder.destroy( m_data );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::buffer() const -> buffer_t const&
{
	return m_data;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::buffer() -> buffer_t&
{
	return m_data;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::memory() const -> concept_t const*
{
	return reinterpret_cast<concept_t const*>( m_data.data() );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::memory() -> concept_t*
{
	return reinterpret_cast<concept_t*>( m_data.data() );
}


template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::replace( concept_t const& src ) -> OnStack&
{
	assert( src.size() <= Size && "OnStack.replace: buffer too small (clone)." );
	destroy();
	src.clone( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
inline auto OnStack<TBuilder, Size, Alignment>::replace( concept_t&& src ) -> OnStack&
{
	assert( src.size() <= Size && "OnStack.replace: buffer too small (extract)." );
	destroy();
	src.extract( memory() );
	return *this;
}
template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline bool OnStack<TBuilder, Size, Alignment>::swap_data( buffer_other_t<Size2>& src ) noexcept
{
	static_assert( IBuilderStack<builder_t, Size>, "OnStack requires IBuilderStack<TBuilder, Size>." );

	auto rsh = m_builder.memory( src );
	if constexpr ( Size2 != Size )
	{
		if ( Size2 < memory()->size() || Size < rsh->size() )
		{
			return false;
		}
	}
	buffer_other_t<Size2> data;
	auto                  tmp = m_builder.memory( data );

	rsh->extract( tmp );
	m_builder.destroy( rsh );
	memory()->extract( rsh );
	replace( std::move( *tmp ) );
	m_builder.destroy( tmp );
	return true;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline bool OnStack<TBuilder, Size, Alignment>::swap( OnStackSrc<Size2>& src ) noexcept
{
	return swap_data( src.m_data );
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment>
template<std::size_t Size2>
inline auto OnStack<TBuilder, Size, Alignment>::swap_unsafe( OnStackSrc<Size2>& src ) -> OnStack&
{
	if constexpr ( Size == Size2 )
	{
		swap_data<Size2>( src.m_data );
	}
	else
	{
		if ( !swap_data<Size2>( src.m_data ) )
		{
			throw std::length_error( "OnStack.swap_unsafe: Buffer too small." );
		}
	}
	return *this;
}

template<IBuilder TBuilder, std::size_t Size, std::size_t Alignment, std::size_t Size2>
inline auto swap( OnStack<TBuilder, Size, Alignment>& lsh, OnStack<TBuilder, Size2, Alignment>& rsh ) noexcept
{
	if constexpr ( Size == Size2 )
	{
		lsh.swap( rsh );
	}
	else
	{
		return lsh.swap( rsh );
	}
}
} // namespace crtp::storage
