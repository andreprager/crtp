#pragma once

#include <tuple>
#include <type_traits>

namespace crtp
{
/// TypeList

template<typename... Ts>
struct TypeList
{};

/// IsTypeList

template<typename T>
struct IsTypeList : std::false_type
{};

template<typename T>
static constexpr bool const IsTypeList_v = IsTypeList<T>::value;

template<typename... Ts>
struct IsTypeList<TypeList<Ts...>> : std::true_type
{};

template<typename T>
concept type_list = IsTypeList_v<T>;

template<typename T>
concept not_type_list = !IsTypeList_v<T>;

/// MakeTuple

/// @brief Build tuple list out of some type @p T.
template<typename T>
struct MakeTuple
{
	using type = std::tuple<T>;
};

template<typename T>
using MakeTuple_t = typename MakeTuple<T>::type;

/// @brief Build tuple out of TypeList<@p Ts...>.
template<typename... Ts>
struct MakeTuple<TypeList<Ts...>>
{
	using type = std::tuple<Ts...>;
};

/// MakeTypeList

/// @brief Build type list out of some type @p T.
template<typename T>
struct MakeTypeList
{
	using type = TypeList<T>;
};

template<typename T>
using MakeTypeList_t = typename MakeTypeList<T>::type;

/// @brief Build type list out of tuple<@p Ts...>.
template<typename... Ts>
struct MakeTypeList<std::tuple<Ts...>>
{
	using type = TypeList<Ts...>;
};

/// TypeListSize

/// @brief Get size of type list @p T.
template<type_list T>
struct TypeListSize : public std::integral_constant<std::size_t, 0>
{};

template<type_list T>
static constexpr std::size_t const TypeListSize_v = TypeListSize<T>::value;
template<type_list T>
static constexpr std::size_t const TypeList_size = TypeListSize<T>::value;

template<typename... Ts>
struct TypeListSize<TypeList<Ts...>> : public std::integral_constant<std::size_t, sizeof...( Ts )>
{};

/// IsTypeIndexValid

/// @brief Check if index is valid for type list @p T.
template<std::size_t Index, type_list T>
    struct IsTypeIndexValid : public std::conditional_t < Index<TypeList_size<T>, std::true_type, std::false_type>
{};

template<std::size_t Index, type_list T>
static constexpr bool const IsTypeIndexValid_v = IsTypeIndexValid<Index, T>::value;

/// FindType

/// @brief  Check if type list @p T contains given typen @p TSearch.
/// @return Index of type @p TSearch if found, otherwise size of type list @p T is returned.
template<typename TSearch, type_list T>
struct FindType : public std::integral_constant<std::size_t, 1>
{};

template<typename TSearch, type_list T>
static constexpr std::size_t const FindType_v = FindType<TSearch, T>::value;

template<typename TSearch>
struct FindType<TSearch, TypeList<>> : public std::integral_constant<std::size_t, 0>
{};

template<typename TSearch, typename T, typename... Ts>
struct FindType<TSearch, TypeList<T, Ts...>>
  : public std::conditional_t<!std::is_same_v<TSearch, T>,
                              std::integral_constant<std::size_t, 1 + FindType_v<TSearch, TypeList<Ts...>>>,
                              std::integral_constant<std::size_t, 0>>
{};

/// HasType

/// @brief Check if type list @p T contains type @p TSearch.
template<typename TSearch, type_list T>
struct HasType : public IsTypeIndexValid<FindType_v<TSearch, T>, T>
{};

template<typename TSearch, type_list T>
static constexpr bool const HasType_v = HasType<TSearch, T>::value;

/// TypeAt

/// @brief Check if type list @p T contains type @p TSearch at position @p Index.
template<std::size_t Index, type_list T>
struct TypeAt : public std::false_type
{
	// using type;
};

template<std::size_t Index, type_list T>
using TypeAt_t = typename TypeAt<Index, T>::type;

template<typename... Ts>
struct TypeAt<0, TypeList<Ts...>> : public std::false_type
{};

template<typename T, typename... Ts>
struct TypeAt<0, TypeList<T, Ts...>> : public std::true_type
{
	using type = T;
};

template<std::size_t Index, typename T, typename... Ts>
struct TypeAt<Index, TypeList<T, Ts...>> : public TypeAt<Index - 1, TypeList<Ts...>>
{};

/// FindTypeIf

/// @brief Check if type list @p T contains type satisfying condition @p TCondition.
/// @return Index of found type if any, otherwise size of type list @p T is returned.
template<template<typename T> typename TCondition, type_list T>
struct FindTypeIf : public std::integral_constant<std::size_t, 1>
{};

/// @brief Index of first found type if any, otherwise size of type list @p T is returned.
template<template<typename> typename TCondition, type_list T>
static constexpr std::size_t const FindTypeIf_v = FindTypeIf<TCondition, T>::value;
/// @brief Get first type satisfying condition @p TCondition.
/// @tparam TCondition Trait to check for types of type list @p T.
/// @pre   Index is valid.
template<template<typename> typename TCondition, type_list T>
using FindTypeIf_t = TypeAt_t<FindTypeIf_v<TCondition, T>, T>;

template<template<typename> typename TCondition, typename... Ts>
struct FindTypeIf<TCondition, TypeList<Ts...>> : public std::integral_constant<std::size_t, 0>
{};

template<template<typename> typename TCondition, typename T, typename... Ts>
struct FindTypeIf<TCondition, TypeList<T, Ts...>>
  : public std::conditional_t<TCondition<T>::value, std::integral_constant<std::size_t, 0>,
                              std::integral_constant<std::size_t, 1 + FindTypeIf_v<TCondition, TypeList<Ts...>>>>
{};

/// TypeListConcat

/// @brief Concat two types to a type list.
template<typename TLhs, typename TRhs>
struct TypeListConcat
{
	using type = TypeList<TLhs, TRhs>;
};

template<typename TLhs, typename TRhs>
using TypeListConcat_t = typename TypeListConcat<TLhs, TRhs>::type;

/// @brief Concat prepend type list @p TRhs... by type @p TLhs.
template<not_type_list TLhs, typename... TRhs>
struct TypeListConcat<TLhs, TypeList<TRhs...>>
{
	using type = TypeList<TLhs, TRhs...>;
};

/// @brief Concat append type list @p TLhs... by type @p TRhs.
template<typename... TLhs, not_type_list TRhs>
struct TypeListConcat<TypeList<TLhs...>, TRhs>
{
	using type = TypeList<TLhs..., TRhs>;
};

/// @brief Concat append type list @p TRhs... by type list @p TRhs...
template<typename... TLhs, typename... TRhs>
struct TypeListConcat<TypeList<TLhs...>, TypeList<TRhs...>>
{
	using type = TypeList<TLhs..., TRhs...>;
};

/// TypeListFlatten

/// @brief Flatten multiple types into a type list without any nested type lists.
template<typename T>
struct TypeListFlatten
{
	using type = TypeList<T>;
};

template<typename T>
using TypeListFlatten_t = typename TypeListFlatten<T>::type;

template<>
struct TypeListFlatten<TypeList<>>
{
	using type = TypeList<>;
};

template<typename T, typename... Ts>
struct TypeListFlatten<TypeList<T, Ts...>>
{
	using lhs_t = std::conditional_t<IsTypeList_v<T>, TypeListFlatten_t<T>, T>;
	using rhs_t = TypeListFlatten_t<TypeList<Ts...>>;
	using type  = TypeListConcat_t<lhs_t, rhs_t>;
};

/// TypeListRemoveAt

/// @brief Remove type at @p Index from type list @p T.
template<std::size_t Index, type_list T>
struct TypeListRemoveAt
{
	using type = TypeList<>;
};

template<std::size_t Index, typename T>
using TypeListRemoveAt_t = typename TypeListRemoveAt<Index, T>::type;

template<>
struct TypeListRemoveAt<0, TypeList<>>
{
	using type = TypeList<>;
};

template<typename T, typename... Ts>
struct TypeListRemoveAt<0, TypeList<T, Ts...>>
{
	using type = TypeList<Ts...>;
};

template<std::size_t Index, typename T, typename... Ts>
struct TypeListRemoveAt<Index, TypeList<T, Ts...>>
{
	using type = TypeListConcat_t<TypeList<T>, TypeListRemoveAt_t<Index - 1, TypeList<Ts...>>>;
};

/// TypeListRemove

/// @brief Remove first type @p TRemove from type list @p TList.
template<typename TRemove, type_list TList>
struct TypeListRemove
{
	using type
	    = std::conditional_t<HasType_v<TRemove, TList>, TypeListRemoveAt_t<FindType_v<TRemove, TList>, TList>, TList>;
};

template<typename TRemove, type_list TList>
using TypeListRemove_t = typename TypeListRemove<TRemove, TList>::type;

/// TypeListRemoveList

/// @brief Remove all types of type list @p TRemove from type list @p TList.
///        Nested type lists are removed completly and not element wise, if any.
template<type_list TRemove, type_list TList>
struct TypeListRemoveList
{
	using type = TList;
};

template<type_list TRemove, type_list TList>
using TypeListRemoveList_t = typename TypeListRemoveList<TRemove, TList>::type;

template<typename T, typename... TLhs, type_list TList>
struct TypeListRemoveList<TypeList<T, TLhs...>, TList>
{
	using type = TypeListRemoveList_t<TypeList<TLhs...>, TypeListRemove_t<T, TList>>;
};

/// TypeListRemoveAll

/// @brief Remove first type @p TRemove from type list @p TList.
template<typename TRemove, type_list TList, typename EnableIf = void>
struct TypeListRemoveAll
{
	using type = TList;
};

template<typename TRemove, type_list TList>
using TypeListRemoveAll_t = typename TypeListRemoveAll<TRemove, TList>::type;

template<typename TRemove, type_list TList>
struct TypeListRemoveAll<TRemove, TList, std::enable_if_t<HasType_v<TRemove, TList>>>
{
	using type = TypeListRemoveAll_t<TRemove, TypeListRemove_t<TRemove, TList>>;
};

} // namespace crtp