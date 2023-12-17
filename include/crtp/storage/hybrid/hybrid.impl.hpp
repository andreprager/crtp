#pragma once

#include "crtp/storage/hybrid/hybrid.decl.hpp"

#include "crtp/storage/detail/concept.impl.hpp"

namespace crtp::storage
{
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
template<typename T>
inline Hybrid<TStorage, TSize, TAlignment>::Hybrid( T value )
{
	if constexpr ( storage_t::template size<T>() <= TSize )
	{
		storage_t::template inplace<TSize, TAlignment>(buffer(), std::move( value ) );
	}
	else
	{
		m_data = storage_t::build( std::move( value ) );
	}
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline Hybrid<TStorage, TSize, TAlignment>::Hybrid( Hybrid const& src )
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

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline Hybrid<TStorage, TSize, TAlignment>::Hybrid( Hybrid&& src ) noexcept
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

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline Hybrid<TStorage, TSize, TAlignment>::~Hybrid()
{
	destroy();
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::operator=( Hybrid const& src ) -> Hybrid&
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

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::operator=( Hybrid&& src ) noexcept -> Hybrid&
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

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void Hybrid<TStorage, TSize, TAlignment>::destroy()
{
	if ( 0 == m_data.index() )
	{
		destroy_buffer();
	}
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void Hybrid<TStorage, TSize, TAlignment>::destroy_buffer()
{
	buffer()->~concept_t();
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void Hybrid<TStorage, TSize, TAlignment>::swap_buffer_pointer( Hybrid& src )
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

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::swap( Hybrid& src ) -> Hybrid&
{
#if 1
	if ( 0 == m_data.index() )
	{
		if ( 0 == src.m_data.index() )
		{
			OnStack<TStorage, TSize, TAlignment>::swap_buffer( buffer(), src.buffer() );
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
#else
	// @TechnicalDebt: seems to not properly move data
	m_data.swap( src.m_data );
	return *this;
#endif
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::buffer() const -> concept_t const*
{
	return reinterpret_cast<concept_t const*>( std::get<0>( m_data ).data() );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::buffer() -> concept_t*
{
	return reinterpret_cast<concept_t*>( std::get<0>( m_data ).data() );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::memory() const -> concept_t const*
{
	return ( 0 == m_data.index() ) ? buffer() : std::get<1>( m_data ).get();
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto Hybrid<TStorage, TSize, TAlignment>::memory() -> concept_t*
{
	return ( 0 == m_data.index() ) ? buffer() : std::get<1>( m_data ).get();
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void swap( Hybrid<TStorage, TSize, TAlignment>& lsh, Hybrid<TStorage, TSize, TAlignment>& rsh )
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
