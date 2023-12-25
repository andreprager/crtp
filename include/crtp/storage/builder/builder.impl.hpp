#pragma once

#include "crtp/storage/builder/builder.decl.hpp"

#include "crtp/storage/concept/concept.impl.hpp"

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
template<std::size_t TSize, std::size_t TAlignment, typename T>
inline void Builder<TModel, TConcept>::inplace( concept_t* memory, T&& value )
{
	using M = model_t<T>;
	static_assert( size<T>() <= TSize, "Size too small for model_t." );
	static_assert( alignof( M ) <= TAlignment, "Alignment too small for model_t." );
	::new ( memory ) M( std::forward<T>( value ) );
}
} // namespace crtp::storage
