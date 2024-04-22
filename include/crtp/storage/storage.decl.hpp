#pragma once

#include "crtp/storage/traits/traits.hpp"

#include <utility>

/// @credits: Klaus Iglberger
///           Inspired creation of this storage part of the library by his talk
///           Breaking Dependencies - C++ Type Erasue, CppCon 2022 and Meeting C++ 2023

/// Global structure and dependencies:
/// Storage
/// +-traits::Policy (OnStack, OnHeap, Hybrid)
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
/// @brief:  Crtp Storage interface provider
/// @tparam: TDerived Concrete class to inject these functions to.
/// @tparam: TPolicy Strategy for concrete storage handling
///   @interface: Required for customized storage policies
///     @function: ctor from som templated value of type T
///     @function: [copy,move]-[ctor,assign]
///     @function: swap function to swap data of underlying concept_t
///     @function: memory function to get underlying concept_t
/// @example: Take a look at the UserApi class in crtp_unit_test project (custom.hpp)
template<typename TDerived, traits::Policy TPolicy>
class Storage : public crtp::Self<TDerived, Storage<TDerived, TPolicy>>
{
public:
	using storage_tag_t = traits::storage_tag;
	using policy_t      = TPolicy;

	/// @brief: Check if underlying storage has some data.
	explicit operator bool() const;
	/// @brief: Swap underlying storage, may throw if not possible.
	bool swap( traits::Storage auto& src ) noexcept;
	void swap_unsafe( traits::Storage auto& src );

	/// @brief: Underlying storage.
	/// @todo:  This should not be public, but is needed for swap, any solution?
	policy_t const& policy() const;
	policy_t&       policy();

	/// @brief: Ctor for different but compatible policy, i.e. same underlying concept.
	Storage( traits::StorageAssignOther<Storage<TDerived, TPolicy>> auto src );
	/// @brief: Value assign for different but compatible policy, i.e. same underlying concept.
	Storage& operator=( traits::StorageAssignOther<Storage<TDerived, TPolicy>> auto src );

protected:
	policy_t m_policy;

private:
	Storage( traits::ValueArgument auto value );

	friend TDerived;
	friend crtp::Self<TDerived, Storage<TDerived, TPolicy>>;
	template<typename TDerived2, traits::Policy TPolicy2>
	friend class Storage;
};

bool swap( traits::Storage auto& lsh, traits::Storage auto& rsh ) noexcept;
bool swap_unsafe( traits::Storage auto& lsh, traits::Storage auto& rsh );

} // namespace crtp::storage
