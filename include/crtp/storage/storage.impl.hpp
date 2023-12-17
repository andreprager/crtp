#pragma once

#include "crtp/storage/storage.decl.hpp"

namespace crtp::storage
{

template<typename TDerived, typename TStoragePolicy>
template<typename T>
inline Storage<TDerived, TStoragePolicy>::Storage( T value ) : m_storage{ std::move( value ) }
{}

template<typename TDerived, typename TStoragePolicy>
inline void* Storage<TDerived, TStoragePolicy>::data()
{
	return m_storage->memory()->data();
}

template<typename TDerived, typename TStoragePolicy>
inline void const* Storage<TDerived, TStoragePolicy>::data() const
{
	return m_storage->memory()->data();
}

template<typename TDerived, typename TStoragePolicy>
template<typename T>
inline T* Storage<TDerived, TStoragePolicy>::cast()
{
	return m_storage->memory()->template cast<T>();
}

template<typename TDerived, typename TStoragePolicy>
template<typename T>
inline T const* Storage<TDerived, TStoragePolicy>::cast() const
{
	return m_storage->memory()->template cast<T>();
}

template<typename TDerived, typename TStoragePolicy>
inline Storage<TDerived, TStoragePolicy>::operator bool() const
{
	return m_storage->memory();
}

template<typename TDerived, typename TStoragePolicy>
inline auto Storage<TDerived, TStoragePolicy>::storage() -> storage_t&
{
	return m_storage;
}

template<typename TDerived, typename TStoragePolicy>
template<typename TSrc, typename TStoragePolicySrc>
inline void Storage<TDerived, TStoragePolicy>::swap( Storage<TSrc, TStoragePolicySrc>& src )
{
	using std::swap;
	swap( m_storage, src.storage() );
}

template<typename T0, typename TStoragePolicy0, typename T1, typename TStoragePolicy1>
inline void swap( Storage<T0, TStoragePolicy0>& lsh, Storage<T1, TStoragePolicy1>& rsh )
{
	lsh.swap( rsh );
}

} // namespace crtp::storage
