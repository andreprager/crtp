#pragma once

#include <utility>

/// @credits: Klaus Iglberger
///           Inspired creation of this storage part of the library by his talk
///           Breaking Dependencies - C++ Type Erasue, CppCon 2022 and Meeting C++ 2023

/// Global structure and dependencies:
/// Storage
/// +-Policy (OnStack, OnHeap, Hybrid)
///   +-Builder
///     +-Concept
///     +-Model (impl of Concept)
///       +-Concept
///     +-value::Concept
///       +-Concept
///     +-value::Model
///       +-Model


namespace crtp::storage
{
/// @brief: Concept for storage policy.
template<typename S>
concept Policy = requires( S s ) {
	0 != sizeof( typename S::concept_t );
	s.swap( std::declval<S&>() );
	{
		std::as_const( s ).memory()
	} -> std::convertible_to<typename S::concept_t const*>;
	{
		s.memory()
	} -> std::convertible_to<typename S::concept_t*>;
};

/// @brief:  Crtp Storage interface provider
/// @tparam: TDerived Concrete class to inject these functions to.
/// @tparam: TPolicy Strategy for concrete storage handling
///   @interface: Required for customized storage policies
///     @function: ctor from som templated value of type T
///     @function: [copy,move]-[ctor,assign]
///     @function: swap function to swap data of underlying concept_t
///     @function: memory function to get underlying concept_t
/// @example: Take a look at the UserApi class in crtp_unit_test project (custom.hpp)
template<typename TDerived, Policy TPolicy>
class Storage : public crtp::Self<TDerived, Storage<TDerived, TPolicy>>
{
public:
	using policy_t = TPolicy;
	/// @brief: Check if underlying storage has some data.
	explicit operator bool() const;
	/// @brief: Swap underlying storage, may throw if not possible.
	template<typename TSrc, typename TPolicySrc>
	void swap( Storage<TSrc, TPolicySrc>& src );

	/// @brief: Underlying storage.
	/// @todo:  This should not be public, but is needed for swap, any solution?
	policy_t const& policy() const;
	policy_t&       policy();

protected:
	policy_t m_policy;

private:
	template<typename T>
	Storage( T value );

	friend TDerived;
	friend crtp::Self<TDerived, Storage<TDerived, TPolicy>>;
};

template<typename T0, typename TPolicy0, typename T1, typename TPolicy1>
void swap( Storage<T0, TPolicy0>& lsh, Storage<T1, TPolicy1>& rsh );

} // namespace crtp::storage
