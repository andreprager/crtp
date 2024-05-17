#pragma once

#include "crtp/storage/concept/concept.decl.hpp"

#include <memory>

namespace crtp::storage
{
/// Model

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline Model<T, TModel, TConcept, TDerived>::Model( T value ) : m_value{ std::move( value ) }
{}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline std::size_t Model<T, TModel, TConcept, TDerived>::size() const
{
	return sizeof( model_t );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void Model<T, TModel, TConcept, TDerived>::clone( clone_t& dst ) const
{
	dst = std::make_unique<model_t>( this->self() );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void Model<T, TModel, TConcept, TDerived>::clone( concept_t* memory ) const
{
	std::construct_at( static_cast<model_t*>( memory ), this->self() );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void Model<T, TModel, TConcept, TDerived>::extract( clone_t& dst ) noexcept
{
	dst = std::make_unique<model_t>( std::move( this->self() ) );
}

template<typename T, typename TModel, IConcept TConcept, typename TDerived>
inline void Model<T, TModel, TConcept, TDerived>::extract( concept_t* memory ) noexcept
{
	std::construct_at( static_cast<model_t*>( memory ), std::move( this->self() ) );
}

/// ModelUnderlying

template<typename T, typename TModel, IConcept TConcept, IConcept TUnderlying, typename TDerived>
inline ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>::ModelUnderlying( T value )
  : base_model_t{ std::move( value ) }
{}

template<typename T, typename TModel, IConcept TConcept, IConcept TUnderlying, typename TDerived>
inline void ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>::clone( underlying_clone_t& dst ) const
{
	dst = std::make_unique<model_t>( this->self() );
}

template<typename T, typename TModel, IConcept TConcept, IConcept TUnderlying, typename TDerived>
inline void ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>::clone( underlying_concept_t* memory ) const
{
	std::construct_at( static_cast<model_t*>( memory ), this->self() );
}

template<typename T, typename TModel, IConcept TConcept, IConcept TUnderlying, typename TDerived>
inline void ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>::extract( underlying_clone_t& dst ) noexcept
{
	dst = std::make_unique<model_t>( std::move(this->self()) );
}

template<typename T, typename TModel, IConcept TConcept, IConcept TUnderlying, typename TDerived>
inline void
    ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>::extract( underlying_concept_t* memory ) noexcept
{
	std::construct_at( static_cast<model_t*>( memory ), std::move(this->self()) );
}

} // namespace crtp::storage
