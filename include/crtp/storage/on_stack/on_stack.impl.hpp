#pragma once

#include "crtp/storage/on_stack/on_stack.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

namespace crtp::storage
{
template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
template<typename T>
inline OnStack<TBuilder, TSize, TAlignment>::OnStack( T value )
{
	static_assert( IBuilderInplace<builder_t, T, TSize, TAlignment>, "OnStack requires IBuilderInplace<TBuilder, T>." );
	builder_t::template inplace<TSize, TAlignment>( memory(), std::move( value ) );
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline OnStack<TBuilder, TSize, TAlignment>::OnStack( OnStack const& src ) : m_data{}
{
	src.memory()->clone( memory() );
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline OnStack<TBuilder, TSize, TAlignment>::OnStack( OnStack&& src ) noexcept : m_data{}
{
	src.memory()->extract( memory() );
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline OnStack<TBuilder, TSize, TAlignment>::~OnStack()
{
	destroy();
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::operator=( OnStack const& src ) -> OnStack&
{
	destroy();
	src.memory()->clone( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::operator=( OnStack&& src ) noexcept -> OnStack&
{
	destroy();
	src.memory()->extract( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void OnStack<TBuilder, TSize, TAlignment>::destroy()
{
	memory()->~concept_t();
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void OnStack<TBuilder, TSize, TAlignment>::swap_buffer( concept_t* lsh, concept_t* rsh )
{
	buffer_t tmp{};
	auto     memory = reinterpret_cast<concept_t*>( tmp.data() );
	lsh->extract( memory );
	rsh->extract( lsh );
	memory->extract( rsh );
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::swap( OnStack& src ) -> OnStack&
{
	swap_buffer( memory(), src.memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::replace( concept_t const & src ) -> OnStack&
{
	destroy();
	src.clone( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::replace( concept_t&& src ) -> OnStack&
{
	destroy();
	src.extract( memory() );
	return *this;
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::memory() const -> concept_t const*
{
	return reinterpret_cast<concept_t const*>( m_data.data() );
}

template<IBuilder TBuilder, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TBuilder, TSize, TAlignment>::memory() -> concept_t*
{
	return reinterpret_cast<concept_t*>( m_data.data() );
}

template<typename TBuilder, std::size_t TSize, std::size_t TAlignment>
inline void swap( OnStack<TBuilder, TSize, TAlignment>& lsh, OnStack<TBuilder, TSize, TAlignment>& rsh )
{
	lsh.swap( rsh );
}
} // namespace crtp::storage
