#pragma once

namespace crtp::storage
{
/// @brief: StoragePolicy
///   @interface:
///     @function: ctor from templated value
///     @function: [copy,move]-[ctor,assign]
///     @function: swap function to swap data of underlying concept_t
///     @function: memory function to get underlying concept_t

/// @brief:  Storage interface provider
/// @tparam: TDerived Concrete class
template<typename TDerived, typename TStoragePolicy>
class Storage : public crtp::Self<TDerived, Storage<TDerived, TStoragePolicy>>
{
public:
	using storage_t = TStoragePolicy;

	void*       data();
	void const* data() const;
	/// @brief: Cast data() pointer to given type T.
	///         Defeat of type system.
	template<typename T>
	T* cast();
	/// @brief: Cast data() pointer to given type T.
	///         Defeat of type system.
	template<typename T>
	T const* cast() const;
	/// @brief: Check if underlying storage as some data.
	explicit operator bool() const;

protected:
	storage_t m_storage;

private:
	template<typename T>
	Storage( T value );
	friend TDerived;
	friend crtp::Self<TDerived, Storage<TDerived, TStoragePolicy>>;
};

} // namespace crtp::storage
