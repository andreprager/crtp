#pragma once

#include "crtp/self.hpp"

#include <cstddef>
#include <memory>

namespace crtp::storage
{
/// @brief:   Crtp for concept interface.
/// @tparam:  TDerived Actual custom interface to inject these interface functions to.
/// @example: Take a look at the UserApiConcept class in crtp_unit_test project (custom.hpp)
template<typename TDerived>
class Concept
{
public:
	using concept_t = TDerived;
	using clone_t   = std::unique_ptr<concept_t>;

	virtual ~Concept() = default;

	/// @brief: Required size in bytes by implementation.
	virtual std::size_t size() const       = 0;
	/// @brief: Clone @this concept by copying concrete internal state to newly creaeted instance.
	///         Similar to copy constructor.
	virtual clone_t     clone() const      = 0;
	/// @brief: Move @this concept by moving concrete internal state to newly created instance.
	///         Similar to move constructor.
	virtual clone_t     extract() noexcept = 0;
	/// @brief: Placement copy, clone @this to @memory.
	///         Similar to copy constructor.
	/// @pre:   @memory must point to a location of at least @size() bytes.
	virtual void clone( concept_t* memory ) const = 0;
	/// @brief: Placement move, move @this to @memory.
	///         Similar to move constructor.
	/// @pre:   @memory must point to a location of at least @size() bytes.
	virtual void extract( concept_t* memory ) noexcept = 0;
	/// @brief: Pointer to some underlying value/state.
	///         Defeat of type system!
	virtual void* data() = 0;
	/// @brief: Pointer (const) to some underlying value/state.
	///         Defeat of type system!
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

	friend TDerived;
};

/// @brief:   Crtp for implementation of concept interface.
/// @tparam:  T        Value type used internally to resolve interface functions.
/// @tparam:  TModel   Actual implementation class of custom interface to inject these concept implementations to.
/// @tparam:  TConcept Acutal custom interface.
/// @example: Take a look at the UserApiModel class in crtp_unit_test project (custom.hpp)
template<typename T, typename TModel, typename TConcept>
class Model
  : private ::crtp::Self<TModel, Model<T, TModel, TConcept>>
  , public TConcept
{
public:
	using concept_t = TConcept;
	using model_t   = TModel;
	using clone_t   = std::unique_ptr<concept_t>;
	std::size_t size() const override;
	clone_t     clone() const override;
	clone_t     extract() noexcept override;
	void clone( concept_t* memory ) const override;
	void extract( concept_t* memory ) noexcept override;

	void*       data() override;
	void const* data() const override;

protected:
	T m_value{};

private:
	Model( T value );
	friend TModel;
	friend crtp::Self<TModel, Model<T, TModel, TConcept>>;
};

/// Trait to identify storage policies
template<typename T>
struct IsStorage : std::false_type
{};
template<typename T>
static constexpr bool IsStorage_v = IsStorage<T>::value;

} // namespace crtp::storage
