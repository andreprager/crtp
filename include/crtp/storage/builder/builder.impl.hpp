#pragma once

#include "crtp/storage/builder/builder.decl.hpp"

namespace crtp::storage
{
template<template<typename> typename TModel, typename TConcept>
template<typename T>
inline auto Builder<TModel, TConcept>::build( T&& value ) -> concept_ptr_t
{
	using M = model_t<T>;
	static_assert( std::is_base_of_v<TConcept, M>, "TConcept must be a base class of TModel." );
	return std::make_unique<M>( std::forward<T>( value ) );
}

template<template<typename> typename TModel, typename TConcept>
template<typename T>
inline constexpr std::size_t Builder<TModel, TConcept>::size()
{
	return sizeof( model_t<T> );
}

template<template<typename> typename TModel, typename TConcept>
template<std::size_t TSize, std::size_t TAlignment, typename T>
inline void Builder<TModel, TConcept>::inplace( concept_t* memory, T&& value )
{
	using M = model_t<T>;
	static_assert( size<T>() <= TSize, "Size too small for model_t." );
	static_assert( alignof( M ) <= TAlignment, "Alignment too small for model_t." );
	::new ( memory ) M( std::forward<T>( value ) );
}
} // namespace crtp::storage
