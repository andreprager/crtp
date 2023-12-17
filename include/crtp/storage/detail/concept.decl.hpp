#pragma once

#include "crtp/self.hpp"

#include <cstddef>
#include <memory>

namespace crtp::storage::detail
{
/// @brief: Crtp for base concept.

template<typename TDerived>
class Concept
{
public:
	using concept_t = TDerived;
	using clone_t   = std::unique_ptr<concept_t>;

	virtual ~Concept() = default;

	virtual std::size_t size() const       = 0;
	virtual clone_t     clone() const      = 0;
	virtual clone_t     extract() noexcept = 0;
	/// @brief: placement copy, clone @this to @memory
	virtual void clone( concept_t* memory ) const = 0;
	/// @brief: placement move, move @this to @memory
	virtual void extract( concept_t* memory ) noexcept = 0;
	/// @brief: Pointer to underlying value.
	///         Defeat of type system.
	virtual void* data() = 0;
	/// @brief: Pointer (const) to underlying value.
	///         Defeat of type system.
	virtual void const* data() const = 0;
	/// @brief: Cast data() pointer to given type T.
	///         Defeat of type system.
	template<typename T>
	T* cast();
	/// @brief: Cast data() pointer to given type T.
	///         Defeat of type system.
	template<typename T>
	T const* cast() const;

private:
	Concept(){};

	friend TDerived;
};

/// @brief:
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
	/// @brief: placement copy, copy self() to @memory
	void clone( concept_t* memory ) const override;
	/// @brief: placement move, move self() to @memory
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

} // namespace crtp::storage::detail

namespace crtp::storage
{
template<typename T>
struct IsStorage : std::false_type
{};
template<typename T>
static constexpr bool IsStorage_v = IsStorage<T>::value;

} // namespace crtp::storage
