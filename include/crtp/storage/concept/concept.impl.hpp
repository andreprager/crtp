#pragma once

#include "crtp/storage/concept/concept.decl.hpp"

namespace crtp::storage
{
/// Concept

template<typename TDerived>
template<typename T>
inline T* Concept<TDerived>::cast()
{
	return reinterpret_cast<T*>( data() );
}

template<typename TDerived>
template<typename T>
inline T const* Concept<TDerived>::cast() const
{
	return reinterpret_cast<T const*>( data() );
}

/// Model

template<typename T, typename TDerived, typename TBase>
inline Model<T, TDerived, TBase>::Model( T value ) : m_value{ std::move( value ) }
{}

template<typename T, typename TDerived, typename TBase>
inline std::size_t Model<T, TDerived, TBase>::size() const
{
	return sizeof(Model);
}

template<typename T, typename TDerived, typename TBase>
inline auto Model<T, TDerived, TBase>::clone() const -> clone_t
{
	return std::make_unique<model_t>( this->self() );
}

template<typename T, typename TDerived, typename TBase>
inline auto Model<T, TDerived, TBase>::extract() noexcept -> clone_t
{
	return std::make_unique<model_t>( std::move( this->self() ) );
}

template<typename T, typename TDerived, typename TBase>
inline void Model<T, TDerived, TBase>::clone( concept_t* memory ) const
{
	::new ( memory ) model_t( this->self() );
}

template<typename T, typename TDerived, typename TBase>
inline void Model<T, TDerived, TBase>::extract( concept_t* memory ) noexcept
{
	::new ( memory ) model_t( std::move( this->self() ) );
}

template<typename T, typename TDerived, typename TBase>
inline void* Model<T, TDerived, TBase>::data()
{
	return &m_value;
}

template<typename T, typename TDerived, typename TBase>
inline void const* Model<T, TDerived, TBase>::data() const
{
	return &m_value;
}

} // namespace crtp::storage
