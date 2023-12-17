#pragma once

#include "crtp/storage/on_stack/on_stack.decl.hpp"

#include "crtp/storage/detail/concept.impl.hpp"

namespace crtp::storage
{
template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
template<typename T>
inline OnStack<TStorage, TSize, TAlignment>::OnStack( T value )
{
	storage_t::template inplace<TSize, TAlignment>(memory(), std::move(value) );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline OnStack<TStorage, TSize, TAlignment>::OnStack( OnStack const& src ) : m_data{}
{
	src.memory()->clone( memory() );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline OnStack<TStorage, TSize, TAlignment>::OnStack( OnStack&& src ) noexcept : m_data{}
{
	src.memory()->extract( memory() );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline OnStack<TStorage, TSize, TAlignment>::~OnStack()
{
	destroy();
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::operator=( OnStack const& src ) -> OnStack&
{
	destroy();
	src.memory()->clone( memory() );
	return *this;
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::operator=( OnStack&& src ) noexcept -> OnStack&
{
	destroy();
	src.memory()->extract( memory() );
	return *this;
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void OnStack<TStorage, TSize, TAlignment>::destroy()
{
	memory()->~concept_t();
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void OnStack<TStorage, TSize, TAlignment>::swap_buffer( concept_t* lsh, concept_t* rsh )
{
	buffer_t tmp{};
	auto     memory = reinterpret_cast<concept_t*>( tmp.data() );
	lsh->extract( memory );
	rsh->extract( lsh );
	memory->extract( rsh );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::swap( OnStack& src ) -> OnStack&
{
	swap_buffer( memory(), src.memory() );
	return *this;
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::replace( concept_t const & src ) -> OnStack&
{
	destroy();
	src.clone( memory() );
	return *this;
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::replace( concept_t&& src ) -> OnStack&
{
	destroy();
	src.extract( memory() );
	return *this;
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::memory() const -> concept_t const*
{
	return reinterpret_cast<concept_t const*>( m_data.data() );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline auto OnStack<TStorage, TSize, TAlignment>::memory() -> concept_t*
{
	return reinterpret_cast<concept_t*>( m_data.data() );
}

template<typename TStorage, std::size_t TSize, std::size_t TAlignment>
inline void swap( OnStack<TStorage, TSize, TAlignment>& lsh, OnStack<TStorage, TSize, TAlignment>& rsh )
{
	lsh.swap( rsh );
}
} // namespace crtp::storage
