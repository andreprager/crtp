#pragma once

#include <concepts>
#include <type_traits>

namespace crtp::storage::traits
{
/// @brief: Concept for storage policy.
template<typename P>
concept Policy = requires( P p )
{
	0 != sizeof( typename P::concept_t );
	p.swap( std::declval<P&>() );
	{
		std::as_const( p ).memory()
	}
	->std::convertible_to<typename P::concept_t const*>;
	{
		p.memory()
	}
	->std::convertible_to<typename P::concept_t*>;
};

template<typename P0, typename P1>
concept PolicyCompatible = Policy<P0>&& Policy<P1>&& std::is_same_v<typename P0::concept_t, typename P1::concept_t>;
template<typename P0, typename P1>
concept PolicyCompatibleOther = PolicyCompatible<P0, P1> && !std::is_same_v<P0, P1>;

template<typename PSrc, typename PDst>
concept PolicyAssign = Policy<PDst>&& Policy<PSrc>&& std::is_base_of_v<typename PDst::concept_t, typename PSrc::concept_t>;
template<typename PSrc, typename PDst>
concept PolicyAssignOther = PolicyAssign<PSrc, PDst> && !std::is_same_v<PDst, PSrc>;

/// @brief: Trait for compatibility of two storage policies.
// template<Policy P0, Policy P1, typename Enable = void>
// struct IsCompatible : std::false_type
//{};
//
// template<Policy P0, Policy P1>
// constexpr bool IsCompatible_v = IsCompatible<P0, P1>::value;
// template<Policy P0, Policy P1>
// constexpr bool IsCompatibleOther_v = !std::is_same_v<P0, P1> && IsCompatible_v<P0, P1>;

/// @brief: Tag to identify storages
struct storage_tag
{};

/// @brief: Concept to check if given type @S is some Storage.
///   Has storage_tag_t type/using equal to storage_tag.
///   Has policy_t member type.
///   Has (const) policy() member function yielding (const) reference to underlying policy.
template<typename S>
concept Storage = requires( S s )
{
	std::is_same_v<storage_tag, typename S::storage_tag_t>;
	0 != sizeof(typename S::policy_t);
	Policy<typename S::policy_t>;
	{
		s.policy()
	}
	->std::convertible_to<typename S::policy_t&>;
	{
		std::as_const( s ).policy()
	}
	->std::convertible_to<typename S::policy_t const&>;
};

template<typename S0, typename S1>
concept StorageCompatible = Storage<S0>&& Storage<S1>&& PolicyCompatible<typename S0::policy_t, typename S1::policy_t>;

template<typename S0, typename S1>
concept StorageCompatibleOther = StorageCompatible<S0, S1> && !std::is_same_v<S0, S1>;

template<typename S, typename P>
concept StoragePolicyCompatible = Storage<S>&& Policy<P>&& PolicyCompatible<typename S::policy_t, P>;

template<typename S, typename P>
concept StoragePolicyCompatibleOther = Storage<S>&& Policy<P>&& PolicyCompatibleOther<typename S::policy_t, P>;

template<typename SSrc, typename SDst>
concept StorageAssign = Storage<SDst>&& Storage<SSrc>&& PolicyAssign<typename SSrc::policy_t, typename SDst::policy_t>;

template<typename SSrc, typename SDst>
concept StorageAssignOther = StorageAssign<SSrc, SDst> && !std::is_same_v<SDst, SSrc>;

template<typename P, typename S>
concept StoragePolicyAssign = Storage<S>&& Policy<P>&& PolicyAssign<P, typename S::policy_t>;

template<typename P, typename S>
concept StoragePolicyAssignOther = Storage<S>&& Policy<P>&& PolicyAssignOther<P, typename S::policy_t>;

template<typename V>
concept ValueArgument = !Storage<V> && !Policy<V>;

} // namespace crtp::storage::traits
