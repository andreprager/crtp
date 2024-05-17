#pragma once

#include "crtp/storage/concept/concept.decl.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace crtp::storage
{
/// @pre: concept_t
/// @pre: clone_t
/// @pre: concept_t* clone_t.operator->()
/// @pre: B.destroy( concept_t* )
template<typename B>
concept IBuilder = requires( B b )
{
	0 != sizeof( typename B::memory_tag_t );
	0 != sizeof( typename B::concept_t );
	0 != sizeof( typename B::clone_t );
	{
		std::declval<typename B::clone_t>().operator->()
	}
	->std::convertible_to<typename B::concept_t*>;
	b.destroy( std::declval<typename B::concept_t*>() );
};

/// @pre: concept_t
/// @pre: clone_t
/// @pre: concept_t* clone_t.operator->()
/// @pre: B.destroy( concept_t* )

/// @pre: B.build<T>( T )
template<typename B, typename T>
concept IBuilderHeap = requires( B b )
{
	IBuilder<B>;
	{
		b.template build<T>( std::declval<T>() )
	}
	->std::convertible_to<typename B::clone_t>;
};

/// @pre: concept_t
/// @pre: clone_t
/// @pre: concept_t* clone_t.operator->()
/// @pre: B.destroy( concept_t* )

/// @pre: B.destroy<Size>( array<byte, Size>& )
/// @pre: concept_t const * B.memory<Size>( array<byte, Size> const& )
/// @pre: concept_t * B.memory<Size>( array<byte, Size>& )
template<typename B, std::size_t Size>
concept IBuilderStack = requires( B b )
{
	IBuilder<B>;
	b.template destroy<Size>( std::declval<std::array<std::byte, Size>&>() );
	{
		b.template memory<Size>( std::declval<std::array<std::byte, Size> const&>() )
	}
	->std::convertible_to<typename B::concept_t const*>;
	{
		b.template memory<Size>( std::declval<std::array<std::byte, Size>&>() )
	}
	->std::convertible_to<typename B::concept_t*>;
};

/// @pre: concept_t
/// @pre: clone_t
/// @pre: concept_t* clone_t.operator->()
/// @pre: B.destroy( concept_t* )
/// @pre: B.destroy<Size>( array<byte, Size>& )
/// @pre: concept_t const * B.memory<Size>( array<byte, Size> const& )
/// @pre: concept_t * B.memory<Size>( array<byte, Size>& )

/// @pre: B.build<T>( T )
/// @pre: size_t B.size<T>( T )
/// @pre: B.inplace<Size, Alignment, T>( concept_t*, T )
template<typename B, std::size_t Size, std::size_t Alignment, typename T>
concept IBuilderInplace = requires( B b )
{
	IBuilderStack<B, Size>;
	{
		b.template size<T>()
	}
	->std::convertible_to<std::size_t>;
	b.template inplace<Size, Alignment, T>( std::declval<typename B::concept_t*>(), std::declval<T>() );
};

/// @brief: Tag to identify if Builder is compatible regarding memory management of model.
struct memory_default_tag
{};

/// @brief: Concept to check for same memory_tag of two builders.
template<typename B0, typename B1>
concept IBuilderMemoryCompatible
    = IBuilder<B0>&& IBuilder<B1>&& std::is_same_v<typename B0::memory_tag_t, typename B1::memory_tag_t>;

/// @brief: Concept to check for compatible underlying concept interface of two builders.
template<typename BDst, typename BSrc>
concept IBuilderCompatible
    = IBuilderMemoryCompatible<BDst, BSrc>&& std::is_base_of_v<typename BDst::concept_t, typename BSrc::concept_t>;

/// @brief: Standard builder strategy
///   @interface: Required for customized builder
///     @type: concept_t defining base class type
///     @function: template<typename T> build(args) to create concrete instance of base class
///     @function: template<typename T> size() Needed size for created model_t<T>.
///     @function: template<typename T> inplace(concept_t *address, args) to create concrete
///                instance of base class inplace at @address with @args.
/// @tparam: TConcept  Base class for instantiated model
/// @tparam: TModel<T> Actual instance created by this builder. Must derive from TConcept.
template<template<typename> typename TModel, IConcept TConcept>
struct Builder
{
	using memory_tag_t = memory_default_tag;
	/// @brief: Actual interface.
	using concept_t = TConcept;
	/// @brief: Pointer to built interface.
	using clone_t = std::unique_ptr<concept_t>;
	/// @brief: Concrete model implementing the interface.
	template<typename T>
	using model_t = TModel<T>;

	/// @brief: Build a new instance for some argument @value.
	template<typename T>
	static clone_t build( T&& value );
	/// @TechnicalDebt: Do we need also a function for alignment?
	/// @brief: Required size for creating an instance with argument of type @T.
	template<typename T>
	static constexpr std::size_t size();
	/// @brief: Create a new instance inplace at address @memory with argument @value.
	/// @pre:   Address@memory must point to a location with size of at least size<T>().
	template<std::size_t Size, std::size_t Alignment, typename T>
	static void inplace( concept_t* memory, T&& value );

	static void destroy( concept_t* memory ) noexcept;
	template<std::size_t Size>
	static void destroy( std::array<std::byte, Size>& src ) noexcept;

	template<std::size_t Size>
	static concept_t const* memory( std::array<std::byte, Size> const& src ) noexcept;
	template<std::size_t Size>
	static concept_t* memory( std::array<std::byte, Size>& src ) noexcept;
};

template<typename T>
struct BuilderConfig
{
	static constexpr std::size_t size  = 0;
	static constexpr std::size_t align = 0;
};

} // namespace crtp::storage
