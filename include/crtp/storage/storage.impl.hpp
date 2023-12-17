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

} // namespace crtp::storage
