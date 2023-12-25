#pragma once

#include "crtp/self.hpp"
#include "crtp/storage/concept/concept.hpp"

#include <concepts>

namespace crtp::storage::value
{
/// @brief: Concept for storage interface.
template<typename C>
concept IConcept = requires( C& c ) {
	::crtp::storage::IConcept<C>;
	{
		c.data()
	} -> std::convertible_to<void*>;
	{
		std::as_const( c ).data()
	} -> std::convertible_to<void const*>;
};

template<typename C, typename T>
concept IConceptCast = requires( C& c ) {
	IConcept<C>;
	{
		c.template cast<T>()
	} -> std::convertible_to<T*>;
	{
		std::as_const( c ).template cast<T>()
	} -> std::convertible_to<T const*>;
};

/// @brief:   Crtp for concept interface.
/// @tparam:  TConcept Actual custom interface to inject these interface functions to.
/// @example: Take a look at the UserApiConcept class in crtp_unit_test project (custom.hpp)
template<typename TConcept, typename TDerived = TConcept>
class Concept : public ::crtp::storage::Concept<TConcept, Concept<TConcept, TDerived>>
{
public:
	using concept_t = TConcept;
	using concept_base_t = ::crtp::storage::Concept<TConcept, Concept>;

	virtual ~Concept() = default;

	/// @brief:   Pointer to some underlying value/state.
	///           Defeat of type system!
	/// @default: nullptr
	virtual void* data() = 0;
	/// @brief:   Pointer (const) to some underlying value/state.
	///           Defeat of type system!
	/// @default: nullptr
	virtual void const* data() const = 0;
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


private:
	Concept(){};

	friend concept_base_t;
	friend TConcept;
};

/// @brief:   Crtp for implementation of concept interface.
/// @tparam:  T        Value type used internally to resolve interface functions.
/// @tparam:  TModel   Actual derived implementation class of custom interface to inject these concept implementations to.
/// @tparam:  TConcept Acutal custom interface.
/// @example: Take a look at the UserApiModel class in crtp_unit_test project (custom.hpp)
template<typename T, typename TModel, IConcept TConcept, typename TDerived = TModel>
class Model
  : private ::crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>
  , public ::crtp::storage::Model<T, TModel, TConcept, Model<T, TModel, TConcept, TDerived>>
{
public:
	using model_base_t = ::crtp::storage::Model<T, TModel, TConcept, Model<T, TModel, TConcept, TDerived>>;
	using concept_t = TConcept;

	void*       data() override;
	void const* data() const override;

private:
	Model( T value );

	friend model_base_t;
	friend TDerived;
	friend ::crtp::Self<TModel, Model<T, TModel, TConcept, TDerived>>;
};

} // namespace crtp::storage::value
