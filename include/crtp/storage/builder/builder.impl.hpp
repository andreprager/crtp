#pragma once

#include "crtp/storage/builder/builder.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

#include <cassert>

namespace crtp::storage
{
template<template<typename> typename TModel, IConcept TConcept>
template<typename T>
inline auto Builder<TModel, TConcept>::build( T&& value ) -> concept_ptr_t
{
	using M = model_t<T>;
	static_assert( std::is_base_of_v<concept_t, M>, "TConcept must be a base class of TModel." );
	static_assert( IConcept<M>, "TConcept must be a base class of TModel." );
	return std::make_unique<M>( std::forward<T>( value ) );
}

template<template<typename> typename TModel, IConcept TConcept>
template<typename T>
inline constexpr std::size_t Builder<TModel, TConcept>::size()
{
	return sizeof( model_t<T> );
}

template<template<typename> typename TModel, IConcept TConcept>
template<std::size_t Size, std::size_t Alignment, typename T>
inline void Builder<TModel, TConcept>::inplace( concept_t* memory, T&& value )
{
	using M = model_t<T>;
	static_assert( size<T>() <= Size, "Size too small for model_t." );
	static_assert( alignof( M ) <= Alignment, "Alignment too small for model_t." );
	std::construct_at<M>( reinterpret_cast<M*>( memory ), std::forward<T>( value ) );
}

template<template<typename> typename TModel, IConcept TConcept>
inline void Builder<TModel, TConcept>::destroy( concept_t* memory ) noexcept
{
	std::destroy_at( memory );
}

template<template<typename> typename TModel, IConcept TConcept>
template<std::size_t Size>
inline void Builder<TModel, TConcept>::destroy( std::array<std::byte, Size>& src ) noexcept
{
	destroy( memory( src ) );
}

template<template<typename> typename TModel, IConcept TConcept>
template<std::size_t Size>
inline auto Builder<TModel, TConcept>::memory( std::array<std::byte, Size> const& memory ) noexcept -> concept_t const*
{
	return reinterpret_cast<concept_t const*>( memory.data() );
}

template<template<typename> typename TModel, IConcept TConcept>
template<std::size_t Size>
inline auto Builder<TModel, TConcept>::memory( std::array<std::byte, Size>& memory ) noexcept -> concept_t*
{
	return reinterpret_cast<concept_t*>( memory.data() );
}

} // namespace crtp::storage
