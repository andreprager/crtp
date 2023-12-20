#pragma once

/// @credits: Klaus Iglberger
///           Inspired creation of this storage part of the library by his talk
///           Breaking Dependencies - C++ Type Erasue, CppCon 2022 and Meeting C++ 2023

namespace crtp::storage
{

/// @brief:  Crtp Storage interface provider
/// @tparam: TDerived Concrete class to inject these functions to.
/// @tparam: TStoragePolicy Strategy for concrete storage handling
///   @interface: Required for customized storage policies
///     @function: ctor from som templated value of type T
///     @function: [copy,move]-[ctor,assign]
///     @function: swap function to swap data of underlying concept_t
///     @function: memory function to get underlying concept_t
/// @example: Take a look at the UserApi class in crtp_unit_test project (custom.hpp)
template<typename TDerived, typename TStoragePolicy>
class Storage : public crtp::Self<TDerived, Storage<TDerived, TStoragePolicy>>
{
public:
	using storage_t = TStoragePolicy;

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
	/// @brief: Check if underlying storage has some data.
	explicit operator bool() const;
	/// @brief: Swap underlying storage, may throw if not possible.
	template<typename TSrc, typename TStoragePolicySrc>
	void swap( Storage<TSrc, TStoragePolicySrc>& src );

	/// @brief: Underlying storage.
	/// @todo:  This should not be public, but is needed for swap, any solution?
	storage_t& storage();

protected:
	storage_t m_storage;

private:
	template<typename T>
	Storage( T value );

	friend TDerived;
	friend crtp::Self<TDerived, Storage<TDerived, TStoragePolicy>>;
};

template<typename T0, typename TStoragePolicy0, typename T1, typename TStoragePolicy1>
void swap( Storage<T0, TStoragePolicy0>& lsh, Storage<T1, TStoragePolicy1>& rsh );

} // namespace crtp::storage
