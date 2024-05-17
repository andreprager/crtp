#pragma once

#include "crtp/self.hpp"

#include <concepts>
#include <cstddef>
#include <memory>
#include <utility>

namespace crtp::storage
{
/// @brief: Concept for storage interface.
template<typename C>
concept IConcept = requires( C c )
{
	0 != sizeof( typename C::concept_t );
	0 != sizeof( typename C::clone_t );
	{
		std::as_const( c ).size()
	}
	->std::convertible_to<std::size_t>;

	{ std::as_const( c ).clone( std::declval<typename C::clone_t&>() ) };
	{ std::as_const( c ).clone( std::declval<C*>() ) };

	{ c.extract( std::declval<typename C::clone_t&>() ) };
	{ c.extract( std::declval<C*>() ) };
};

/// Concept

/// @brief:   Crtp for concept interface.
/// @tparam:  TConcept Actual custom interface to inject these interface functions to.
/// @example: Take a look at the UserApiConcept class in crtp_unit_test project (custom.hpp)
template<typename TConcept, typename TDerived = TConcept>
class Concept
{
public:
	using concept_t = TConcept;
	using clone_t   = std::unique_ptr<concept_t>;

	virtual ~Concept() = default;

	/// @brief: Required size in bytes by implementation.
	virtual std::size_t size() const = 0;
	/// @brief: Clone @this concept by copying concrete internal state to newly creaeted instance.
	///         Similar to copy constructor.
	/// @post:  this is not destroyed, just copied from.
	virtual void clone( clone_t& dst ) const = 0;
	/// @brief: Placement copy, clone @this to @memory.
	///         Similar to copy constructor.
	/// @pre:   @memory must point to a location of at least @size() bytes.
	/// @pre:   @memory must be not constructed yet.
	/// @post:  this is not destroyed, just copied from.
	virtual void clone( concept_t* memory ) const = 0;
	/// @brief: Move @this concept by moving concrete internal state to newly created instance.
	///         Similar to move constructor.
	/// @post:  this is not destroyed, just moved from.
	virtual void extract( clone_t& dst ) noexcept = 0;
	/// @brief: Placement move, move @this to @memory.
	///         Similar to move constructor.
	/// @pre:   @memory must point to a location of at least @size() bytes.
	/// @pre:   @memory must be not constructed yet.
	/// @post:  this is not destroyed, just moved from.
	virtual void extract( concept_t* memory ) noexcept = 0;

private:
	Concept(){};

	friend TDerived;
};

/// ConceptUnderlying

/// @brief: Convenience class for concept with a base concept.
///         Makes all interface functions visible.
///         Declares necessary concept types.
template<typename TConcept, IConcept TUnderlying, typename TDerived = TConcept>
class ConceptUnderlying
  : public TUnderlying // has to be first for proper memory layout
  , public Concept<TConcept, ConceptUnderlying<TConcept, TUnderlying, TDerived>>
{
public:
	using base_concept_t = Concept<TConcept, ConceptUnderlying<TConcept, TUnderlying, TDerived>>;
	using typename base_concept_t::clone_t;
	using typename base_concept_t::concept_t;

	using base_concept_t::clone;
	using base_concept_t::extract;
	using base_concept_t::size;
	using TUnderlying::clone;
	using TUnderlying::extract;

private:
	ConceptUnderlying(){};

	friend TDerived;
};

/// @brief:   Crtp for implementation of concept interface.
/// @tparam:  T        Value type used internally to resolve interface functions.
/// @tparam:  TModel   Actual derived implementation class of custom interface to inject these concept implementations
/// to.
/// @tparam:  TConcept Acutal custom interface.
/// @tparam:  TDerived Immediate parent class deriving from this Model.
/// @example: Take a look at the UserApiModel class in crtp_unit_test project (custom.hpp)
template<typename T, typename TModel, IConcept TConcept, typename TDerived = TModel>
class Model
  : public TConcept
  , private ::crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>
{
public:
	using base_concept_t = TConcept;
	using concept_t      = base_concept_t::concept_t;
	using clone_t        = base_concept_t::clone_t;
	using model_t        = TModel;
	std::size_t size() const override;
	void        clone( clone_t& dst ) const override;
	void        clone( concept_t* memory ) const override;
	void        extract( clone_t& dst ) noexcept override;
	void        extract( concept_t* memory ) noexcept override;

protected:
	T m_value{};

private:
	Model( T value );
	friend TDerived;
	friend crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>;
};

/// ModelUnderlying

template<typename T, typename TModel, IConcept TConcept, IConcept TUnderlying, typename TDerived = TModel>
class ModelUnderlying : public Model<T, TModel, TConcept, ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>>
{
public:
	static_assert( std::is_base_of_v<TUnderlying, TConcept>, "TUnderlying concept must be a base of TConcept" );
	using base_model_t = Model<T, TModel, TConcept, ModelUnderlying<T, TModel, TConcept, TUnderlying, TDerived>>;
	using typename base_model_t::base_concept_t;
	using typename base_model_t::clone_t;
	using typename base_model_t::concept_t;
	using typename base_model_t::model_t;

	using underlying_base_t    = TUnderlying;
	using underlying_concept_t = underlying_base_t::concept_t;
	using underlying_clone_t   = underlying_base_t::clone_t;

	void clone( underlying_clone_t& dst ) const override;
	void clone( underlying_concept_t* memory ) const override;
	void extract( underlying_clone_t& dst ) noexcept override;
	void extract( underlying_concept_t* memory ) noexcept override;

	using base_model_t::clone;
	using base_model_t::extract;

private:
	ModelUnderlying( T value );
	friend TDerived;
	friend crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>;
};

} // namespace crtp::storage
