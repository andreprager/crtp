#pragma once

#include "crtp/storage/value/value.decl.hpp"

namespace crtp::storage::value
{
/// Value

template<typename TDerived, Policy TPolicy>
inline Value<TDerived, TPolicy>::Value( traits::ValueArgument auto value ) : storage_base_t{ std::move( value ) }
{}

template<typename TDerived, Policy TPolicy>
inline void* Value<TDerived, TPolicy>::data()
{
	return this->policy().memory()->data();
}

template<typename TDerived, Policy TPolicy>
inline void const* Value<TDerived, TPolicy>::data() const
{
	return this->policy().memory()->data();
}

template<typename TDerived, Policy TPolicy>
template<typename T>
inline T* Value<TDerived, TPolicy>::cast()
{
	static_assert(PolicyCast<TPolicy, T>, "Value requires PolicyCast<TPolicy, T>.");
	return this->policy().memory()->template cast<T>();
}

template<typename TDerived, Policy TPolicy>
template<typename T>
inline T const* Value<TDerived, TPolicy>::cast() const
{
	static_assert(PolicyCast<TPolicy, T>, "Value requires PolicyCast<TPolicy, T>.");
	return this->policy().memory()->template cast<T>();
}

} // namespace crtp::storage::value
