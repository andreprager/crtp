#pragma once

#include "crtp/storage/concept/concept.decl.hpp"

namespace crtp::storage
{
/// Model

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline Model<T, TModel, TConcept, TDerived>::Model( T value ) : m_value{ std::move( value ) }
{}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline std::size_t Model<T, TModel, TConcept, TDerived>::size() const
{
	return sizeof(Model);
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline auto Model<T, TModel, TConcept, TDerived>::clone() const -> clone_t
{
	return std::make_unique<model_t>( this->self() );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void Model<T, TModel, TConcept, TDerived>::clone( concept_t* memory ) const
{
	::new ( memory ) model_t( this->self() );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline auto Model<T, TModel, TConcept, TDerived>::extract() noexcept -> clone_t
{
	return std::make_unique<model_t>( std::move( this->self() ) );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void Model<T, TModel, TConcept, TDerived>::extract( concept_t* memory ) noexcept
{
	::new ( memory ) model_t( std::move( this->self() ) );
}

} // namespace crtp::storage
