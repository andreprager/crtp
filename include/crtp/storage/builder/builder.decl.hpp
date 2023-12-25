#pragma once

#include "crtp/storage/concept/concept.decl.hpp"

#include <cstddef>
#include <memory>
#include <utility>

namespace crtp::storage
{
template<typename B>
concept IBuilder = requires() {
	0 != sizeof( typename B::concept_t );
	0 != sizeof( typename B::concept_ptr_t );
	{
		std::declval<typename B::concept_ptr_t>().operator->()
	} -> std::convertible_to<typename B::concept_t*>;
	// how do check for static build, size and inplace functions?
};

template<typename B, typename T>
concept IBuilderHeap = requires() {
	IBuilder<B>;
	{
		B::template build<T>( std::declval<T>() )
	} -> std::convertible_to<typename B::concept_ptr_t>;
};

template<typename B, typename T, std::size_t TSize, std::size_t TAlignment>
concept IBuilderInplace = requires() {
	IBuilder<B>;
	{
		B::template size<T>()
	} -> std::convertible_to<std::size_t>;
	B::template inplace<TSize, TAlignment, T>( std::declval<typename B::concept_t*>(), std::declval<T>() );
};

/// @brief: Standard builder strategy
///   @interface: Required for customized builder
///     @type: concept_t defining base class type
///     @function: template<typename T> build(args) to create concrete instance of base class
///     @function: template<typename T> size(args) Needed size for object created with @args
///     @function: template<typename T> inplace(concept_t *address, args) to create concrete
///                instance of base class inplace at @address with @args.
/// @tparam: TConcept  Base class for instantiated model
/// @tparam: TModel<T> Actual instance created by this builder. Must derive from TConcept.
template<template<typename> typename TModel, IConcept TConcept>
struct Builder
{
	/// @brief: Actual interface.
	using concept_t = TConcept;
	/// @brief: Pointer to built interface.
	using concept_ptr_t = std::unique_ptr<concept_t>;
	/// @brief: Concrete model implementing the interface.
	template<typename T>
	using model_t = TModel<T>;

	/// @brief: Build a new instance for some argument @value.
	template<typename T>
	static concept_ptr_t build( T&& value );
	/// @TechnicalDebt: Do we need also a function for alignment?
	/// @brief: Required size for creating an instance with argument of type @T.
	template<typename T>
	static constexpr std::size_t size();
	/// @brief: Create a new instance inplace at address @memory with argument @value.
	/// @pre:   Address@memory must point to a location with size of at least size<T>().
	template<std::size_t TSize, std::size_t TAlignment, typename T>
	static void inplace( concept_t* memory, T&& value );
};

} // namespace crtp::storage
