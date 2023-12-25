#pragma once

#include "crtp/storage/storage.decl.hpp"

namespace crtp::storage
{

template<typename TDerived, Policy TPolicy>
template<typename T>
inline Storage<TDerived, TPolicy>::Storage( T value ) : m_policy{ std::move( value ) }
{}

template<typename TDerived, Policy TPolicy>
inline Storage<TDerived, TPolicy>::operator bool() const
{
	return m_policy.memory();
}

template<typename TDerived, Policy TPolicy>
inline auto Storage<TDerived, TPolicy>::policy() const -> policy_t const&
{
	return m_policy;
}

template<typename TDerived, Policy TPolicy>
inline auto Storage<TDerived, TPolicy>::policy() -> policy_t&
{
	return m_policy;
}

template<typename TDerived, Policy TPolicy>
template<typename TSrc, typename TPolicySrc>
inline void Storage<TDerived, TPolicy>::swap( Storage<TSrc, TPolicySrc>& src )
{
	using std::swap;
	swap( m_policy, src.policy() );
}

template<typename T0, Policy TPolicy0, typename T1, Policy TPolicy1>
inline void swap( Storage<T0, TPolicy0>& lsh, Storage<T1, TPolicy1>& rsh )
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
