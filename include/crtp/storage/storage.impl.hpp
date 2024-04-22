#pragma once

#include "crtp/storage/storage.decl.hpp"

#include "crtp/storage/swap/swap.decl.hpp"

namespace crtp::storage
{
template<typename TDerived, traits::Policy TPolicy>
inline Storage<TDerived, TPolicy>::Storage( traits::ValueArgument auto value ) : m_policy{ std::move( value ) }
{}

template<typename TDerived, traits::Policy TPolicy>
inline Storage<TDerived, TPolicy>::Storage( traits::StorageAssignOther<Storage<TDerived, TPolicy>> auto src )
  : m_policy{ std::move( src.m_policy ) }
{}

template<typename TDerived, traits::Policy TPolicy>
inline auto Storage<TDerived, TPolicy>::operator=( traits::StorageAssignOther<Storage<TDerived, TPolicy>> auto src )
    -> Storage&
{
	m_policy = std::move( src.m_policy );
	return *this;
}

template<typename TDerived, traits::Policy TPolicy>
inline Storage<TDerived, TPolicy>::operator bool() const
{
	return m_policy.memory();
}

template<typename TDerived, traits::Policy TPolicy>
inline auto Storage<TDerived, TPolicy>::policy() const -> policy_t const&
{
	return m_policy;
}

template<typename TDerived, traits::Policy TPolicy>
inline auto Storage<TDerived, TPolicy>::policy() -> policy_t&
{
	return m_policy;
}

template<typename TDerived, traits::Policy TPolicy>
inline bool Storage<TDerived, TPolicy>::swap( traits::Storage auto& src ) noexcept
{
	using crtp::storage::swap;
	if constexpr ( std::is_same_v<void, decltype( swap( m_policy, src.policy() ) )> )
	{
		swap( m_policy, src.policy() );
		return true;
	}
	else
	{
		return swap( m_policy, src.policy() );
	}
}

template<typename TDerived, traits::Policy TPolicy>
inline void Storage<TDerived, TPolicy>::swap_unsafe( traits::Storage auto& src )
{
	swap_unsafe( m_policy, src.policy() );
}

inline bool swap( traits::Storage auto& lsh, traits::Storage auto& rsh ) noexcept
{
	return lsh.swap( rsh );
}

inline void swap_unsafe( traits::Storage auto& lsh, traits::Storage auto& rsh )
{
	lsh.swap_unsafe( rsh );
}

} // namespace crtp::storage
