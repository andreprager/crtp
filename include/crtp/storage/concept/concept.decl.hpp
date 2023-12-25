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
concept IConcept = requires( C& c ) {
	{
		std::as_const( c ).size()
	} -> std::convertible_to<std::size_t>;

	{
		std::as_const( c ).clone()
	} -> std::convertible_to<std::unique_ptr<typename C::concept_t>>;
	std::as_const( c ).clone( std::declval<C*>() );

	{
		c.extract()
	} -> std::convertible_to<std::unique_ptr<typename C::concept_t>>;
	c.extract( std::declval<C*>() );
};

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
	virtual clone_t clone() const = 0;
	/// @brief: Placement copy, clone @this to @memory.
	///         Similar to copy constructor.
	/// @pre:   @memory must point to a location of at least @size() bytes.
	virtual void clone( concept_t* memory ) const = 0;
	/// @brief: Move @this concept by moving concrete internal state to newly created instance.
	///         Similar to move constructor.
	virtual clone_t extract() noexcept = 0;
	/// @brief: Placement move, move @this to @memory.
	///         Similar to move constructor.
	/// @pre:   @memory must point to a location of at least @size() bytes.
	virtual void extract( concept_t* memory ) noexcept = 0;

private:
	Concept(){};

	friend TDerived;
};

/// @brief:   Crtp for implementation of concept interface.
/// @tparam:  T        Value type used internally to resolve interface functions.
/// @tparam:  TModel   Actual derived implementation class of custom interface to inject these concept implementations to.
/// @tparam:  TConcept Acutal custom interface.
/// @tparam:  TDerived Immediate parent class deriving from this Model.
/// @example: Take a look at the UserApiModel class in crtp_unit_test project (custom.hpp)
template<typename T, typename TModel, IConcept TConcept, typename TDerived = TModel>
class Model
  : private ::crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>
  , public TConcept
{
public:
	using concept_t = TConcept;
	using model_t   = TModel;
	using clone_t   = std::unique_ptr<concept_t>;
	std::size_t size() const override;
	clone_t     clone() const override;
	void        clone( concept_t* memory ) const override;
	clone_t     extract() noexcept override;
	void        extract( concept_t* memory ) noexcept override;

protected:
	T m_value{};

private:
	Model( T value );
	friend TDerived;
	friend crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>;
};

} // namespace crtp::storage
