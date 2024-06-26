#pragma once

#include "crtp/self.hpp"
#include "crtp/storage/storage.hpp"
#include "crtp/storage/traits/traits.hpp"
#include "crtp/storage/value/concept/concept.hpp"

#include <concepts>

namespace crtp::storage::value
{
/// @brief: Concept for value policy.
template<typename P>
concept Policy = requires( P p ) {
	::crtp::storage::traits::Policy<P>;
	IConcept<P>;
};

/// @brief: Concept for va�ue cast policy.
template<typename P, typename T>
concept PolicyCast = requires( P p ) {
	Policy<P>;
	IConceptCast<P, T>;
};

/// @brief:  Crtp Storage value interface provider
/// @tparam: TDerived Concrete class to inject these functions to.
/// @tparam: TPolicy Strategy for concrete storage handling
///   @interface: Required for customized storage policies
///     @function: ctor from som templated value of type T
///     @function: [copy,move]-[ctor,assign]
///     @function: swap function to swap data of underlying concept_t
///     @function: memory function to get underlying concept_t
/// @example: Take a look at the UserApi class in crtp_unit_test project (custom.hpp)
template<typename TDerived, Policy TPolicy>
class Value : public ::crtp::storage::Storage<Value<TDerived, TPolicy>, TPolicy>
{
public:
	using storage_base_t = ::crtp::storage::Storage<Value<TDerived, TPolicy>, TPolicy>;

	/// @brief: Pointer to some underlying value/state.
	///         Defeat of type system!
	void* data();
	/// @brief: Pointer to some underlying value/state.
	///         Defeat of type system!
	void const* data() const;
	/// @brief: Cast @data() to given type T.
	///         Defeat of type system!
	/// @pre:   Cast must be valid.
	template<typename T>
	T* cast();
	/// @brief: Cast @data() to given type T.
	///         Defeat of type system!
	/// @pre:   Cast must be valid.
	template<typename T>
	T const* cast() const;

private:
	Value( traits::ValueArgument auto value );

	friend TDerived;
	friend storage_base_t;
};

} // namespace crtp::storage::value
