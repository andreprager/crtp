#pragma once

#include "crtp/storage/value/concept/concept.decl.hpp"

namespace crtp::storage::value
{
/// Concept

template<typename TConcept, typename TDerived>
template<typename T>
inline T* Concept<TConcept, TDerived>::cast()
{
	return reinterpret_cast<T*>( data() );
}

template<typename TConcept, typename TDerived>
template<typename T>
inline T const* Concept<TConcept, TDerived>::cast() const
{
	return reinterpret_cast<T const*>( data() );
}

/// Model

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline Model<T, TModel, TConcept, TDerived>::Model( T value ) : model_base_t{ std::move( value ) }
{}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void* Model<T, TModel, TConcept, TDerived>::data()
{
	return &this->m_value;
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void const* Model<T, TModel, TConcept, TDerived>::data() const
{
	return &this->m_value;
}

} // namespace crtp::storage::value
