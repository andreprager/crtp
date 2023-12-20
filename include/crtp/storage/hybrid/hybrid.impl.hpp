#pragma once

#include "crtp/storage/hybrid/hybrid.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

namespace crtp::storage
{
template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
template<typename T>
inline Hybrid<TBuilder, TSize, TAlignment>::Hybrid( T value )
{
	if constexpr ( builder_t::template size<T>() <= TSize )
	{
		builder_t::template inplace<TSize, TAlignment>(buffer(), std::move( value ) );
	}
	else
	{
		m_data = builder_t::build( std::move( value ) );
	}
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline Hybrid<TBuilder, TSize, TAlignment>::Hybrid( Hybrid const& src )
{
	if ( 0 == src.m_data.index() )
	{
		src.memory()->clone( buffer() );
	}
	else
	{
		m_data = std::get<1>( src.m_data )->clone();
	}
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline Hybrid<TBuilder, TSize, TAlignment>::Hybrid( Hybrid&& src ) noexcept
{
	if ( 0 == src.m_data.index() )
	{
		src.memory()->extract( buffer() );
	}
	else
	{
		m_data = std::get<1>( src.m_data )->extract();
	}
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline Hybrid<TBuilder, TSize, TAlignment>::~Hybrid()
{
	destroy();
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::operator=( Hybrid const& src ) -> Hybrid&
{
	if ( 0 == src.m_data.index() )
	{
		destroy_buffer();
		src.memory()->clone( buffer() );
	}
	else
	{
		m_data = std::get<1>( src.m_data )->clone();
	}
	return *this;
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::operator=( Hybrid&& src ) noexcept -> Hybrid&
{
	if ( 0 == src.m_data.index() )
	{
		destroy_buffer();
		src.memory()->extract( buffer() );
	}
	else
	{
		m_data = std::get<1>( src.m_data )->extract();
	}
	return *this;
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void Hybrid<TBuilder, TSize, TAlignment>::destroy()
{
	if ( 0 == m_data.index() )
	{
		destroy_buffer();
	}
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void Hybrid<TBuilder, TSize, TAlignment>::destroy_buffer()
{
	buffer()->~concept_t();
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void Hybrid<TBuilder, TSize, TAlignment>::swap_buffer_pointer( Hybrid& src )
{
	// save @src unique_ptr
	std::unique_ptr<concept_t> unique = std::move( std::get<1>( src.m_data ) );
	// move this buffer concept to @src
	src.m_data.emplace<0>();
	buffer()->extract( src.buffer() );
	destroy_buffer();
	// move saved @src unique_ptr to this
	src.m_data = std::move( unique );
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::swap( Hybrid& src ) -> Hybrid&
{
	if ( 0 == m_data.index() )
	{
		if ( 0 == src.m_data.index() )
		{
			OnStack<TBuilder, TSize, TAlignment>::swap_buffer( buffer(), src.buffer() );
			return *this;
		}
		swap_buffer_pointer( src );
		return *this;
	}
	if ( 0 == src.m_data.index() )
	{
		src.swap_buffer_pointer( *this );
		return *this;
	}
	std::get<1>( m_data ).swap( std::get<1>( src.m_data ) );
	return *this;
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::buffer() const -> concept_t const*
{
	return reinterpret_cast<concept_t const*>( std::get<0>( m_data ).data() );
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::buffer() -> concept_t*
{
	return reinterpret_cast<concept_t*>( std::get<0>( m_data ).data() );
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::memory() const -> concept_t const*
{
	return ( 0 == m_data.index() ) ? buffer() : std::get<1>( m_data ).get();
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TBuilder, TSize, TAlignment>::memory() -> concept_t*
{
	return ( 0 == m_data.index() ) ? buffer() : std::get<1>( m_data ).get();
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void swap( Hybrid<TBuilder, TSize, TAlignment>& lsh, Hybrid<TBuilder, TSize, TAlignment>& rsh )
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
