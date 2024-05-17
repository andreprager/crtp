#ifndef utility_custom_hpp
#define utility_custom_hpp

#include <crtp/self.hpp>
#include <crtp/storage/builder/builder.hpp>
#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/storage.hpp>
#include <crtp/storage/value/value.hpp>

#include <array>
#include <memory>
#include <numeric>
#include <ostream>
#include <string>
#include <vector>

namespace custom
{
/// UserApiConcept

/// @brief: Customized Concept with some custom interface function user_api.
/// @customize: Define interface once.
class UserApiConcept : public crtp::storage::Concept<UserApiConcept>
{
public:
	using base_concept_t = crtp::storage::Concept<UserApiConcept>;
	using typename base_concept_t::clone_t;
	using typename base_concept_t::concept_t;

	/// @brief: customizable interface functions
	/// @customize
	virtual void user_api() const = 0;
};

inline void call_user_api( UserApiConcept const& src )
{
	src.user_api();
}

/// UserApiConceptDerived

class UserApiConceptDerived
  : public crtp::storage::ConceptUnderlying<UserApiConceptDerived, UserApiConcept>
{
public:
	using base_concept_t = crtp::storage::ConceptUnderlying<UserApiConceptDerived, UserApiConcept>;

	/// @brief: customizable interface functions
	/// @customize
	virtual void user_api_derived() const = 0;
};

inline void call_user_api_derived( UserApiConceptDerived const& src )
{
	src.user_api_derived();
}

/// UserApiModel

/// @brief: Customized Model for some value of Type T.
/// @pre:   Function call_user_api(T) must exist.
/// @customize: Define model once, implementing interface.
template<typename T>
class UserApiModel : public crtp::storage::Model<T, UserApiModel<T>, UserApiConcept>
{
public:
	using base_model_t = crtp::storage::Model<T, UserApiModel<T>, UserApiConcept>;
	UserApiModel( T value ) : base_model_t{ std::move( value ) }
	{}

	/// @customize
	void user_api() const override
	{
		call_user_api( this->m_value );
	}
};

/// UserApiModelDerived

/// @brief: Customized Model for some value of Type T.
/// @pre:   Function call_user_api_derived(T) must exist.
/// @customize: Define model once, implementing interface.
template<typename T>
class UserApiModelDerived
  : public crtp::storage::ModelUnderlying<T, UserApiModelDerived<T>, UserApiConceptDerived, UserApiConcept>
{
public:
	using base_model_t = crtp::storage::ModelUnderlying<T, UserApiModelDerived<T>, UserApiConceptDerived, UserApiConcept>;
	UserApiModelDerived( T value ) : base_model_t{ std::move( value ) }
	{}

	/// @customize
	void user_api() const override
	{
		call_user_api( this->m_value );
	}

	/// @customize
	void user_api_derived() const override
	{
		call_user_api_derived( this->m_value );
	}
};

/// UserApi

/// @brief: Actual type erased value with some storage policy.
/// @pre:   TPolicy concept_t must have a user_api function.
/// @customize: Default value implementation, accepting all value types.
template<typename TPolicy>
class UserApi : public crtp::storage::Storage<UserApi<TPolicy>, TPolicy>
{
public:
	using storage_t = crtp::storage::Storage<UserApi<TPolicy>, TPolicy>;

	/// @brief:
	/// @pre:   call_user_api function
	template<typename T>
	UserApi( T value ) : storage_t{ std::move( value ) }
	{}

	void user_api() const
	{
		this->m_policy.memory()->user_api();
	}

	template<typename T>
	friend void call_user_api( UserApi<T> const& src );
};

template<typename T>
void call_user_api( UserApi<T> const& src )
{
	src.user_api();
}

/// UserApiDerived

/// @brief: Actual type erased value with some storage policy.
/// @pre:   TPolicy concept_t must have a user_api_derived function.
/// @customize: Default value implementation, accepting all value types.
template<typename TPolicy>
class UserApiDerived : public crtp::storage::Storage<UserApiDerived<TPolicy>, TPolicy>
{
public:
	using storage_t = crtp::storage::Storage<UserApiDerived<TPolicy>, TPolicy>;

	/// @brief:
	/// @pre:   call_user_api function
	template<typename T>
	UserApiDerived( T value ) : storage_t{ std::move( value ) }
	{}

	void user_api() const
	{
		this->m_policy.memory()->user_api();
	}

	void user_api_derived() const
	{
		this->m_policy.memory()->user_api_derived();
	}

private:
	template<typename T>
	friend void call_user_api( UserApiDerived<T> const& src );
	template<typename T>
	friend void call_user_api_derived( UserApiDerived<T> const& src );
};

template<typename T>
void call_user_api( UserApiDerived<T> const& src )
{
	src.user_api();
}

template<typename T>
void call_user_api_derived( UserApiDerived<T> const& src )
{
	src.user_api_derived();
}

/// @brief: Default builder strategy for UserApi
using UserApiBuilder        = crtp::storage::Builder<UserApiModel, UserApiConcept>;
using UserApiBuilderDerived = crtp::storage::Builder<UserApiModelDerived, UserApiConceptDerived>;

/// Manual Virtual Dispatch for UserApi

class ViewUserApi
{
public:
	template<typename TAny>
	ViewUserApi( TAny const& value )
	  : m_value{ std::addressof( value ) }
	  , m_user_api{ []( void const* value ) { call_user_api( *static_cast<TAny const*>( value ) ); } }
	{}

	friend void call_user_api( ViewUserApi const& view )
	{
		view.m_user_api( view.m_value );
	}

private:
	using user_api_t = void( void const* );

	void const* m_value{};
	user_api_t* m_user_api{};
};

/// UserValueConcept

/// @brief: Optional customized Concept for value. value::Concept may be used directly
/// @customize: Define interface once.
class UserValueConcept : public crtp::storage::value::Concept<UserValueConcept>
{
public:
	using base_concept_t = crtp::storage::value::Concept<UserValueConcept>;
	using typename base_concept_t::clone_t;
	using typename base_concept_t::concept_t;
};

/// UserValueModel

/// @brief: Optional customized Model for some value of Type T.
/// @customize: Define model once, implementing interface.
template<typename T>
class UserValueModel : public crtp::storage::value::Model<T, UserValueModel<T>, UserValueConcept>
{
public:
	using model_t = crtp::storage::value::Model<T, UserValueModel<T>, UserValueConcept>;
	UserValueModel( T value ) : model_t{ std::move( value ) }
	{}
};

/// UserValue

/// @brief: Actual type erased value with some storage policy.
/// @pre:   TPolicy concept_t must have a user_api function.
/// @customize: Default value implementation, accepting all value types.
template<typename TPolicy>
class UserValue : public crtp::storage::value::Value<UserValue<TPolicy>, TPolicy>
{
public:
	using storage_t = crtp::storage::value::Value<UserValue<TPolicy>, TPolicy>;

	/// @brief:
	template<typename T>
	UserValue( T value ) : storage_t{ std::move( value ) }
	{}
};

/// @brief: Default builder strategy for UserValue
using UserValueBuilder = crtp::storage::Builder<UserValueModel, UserValueConcept>;

/// Array

/// @brief: Class to simulate some big stack type with some alignment
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
struct Array
{
	using range_t = std::array<std::uint8_t, TSize>;

	auto begin() const
	{
		return m_data.begin();
	}
	auto begin()
	{
		return m_data.begin();
	}
	auto end() const
	{
		return m_data.end();
	}
	auto end()
	{
		return m_data.end();
	}

	static Array sequence( std::size_t size, std::uint8_t offset = {} )
	{
		Array ret;
		std::iota( ret.begin(), ret.end(), offset );
		return ret;
	}

	operator range_t const &() const&
	{
		return m_data;
	};
	operator range_t&&() &&
	{
		return std::move( m_data );
	};

	operator range_t&() &
	{
		return m_data;
	};

	alignas( TAlignment ) range_t m_data;
};

template<std::size_t TSize = 128, std::size_t TAlignment = 16>
bool operator==( Array<TSize, TAlignment> const& lhs, Array<TSize, TAlignment> const& rhs );
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
bool operator!=( Array<TSize, TAlignment> const& lhs, Array<TSize, TAlignment> const& rhs );

/// @brief: Track last call to call_user_api with some Array
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
Array<TSize, TAlignment> gs_array;
/// @brief: Track last call to call_user_api_derived with some Array
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
Array<TSize, TAlignment> gs_array_derived;

/// @brief: Allows creating UserApiModel with Array
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
inline void call_user_api( Array<TSize, TAlignment> const& src )
{
	gs_array<TSize, TAlignment> = src;
}

/// @brief: Allows creating UserApiModelDerived with Array
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
inline void call_user_api_derived( Array<TSize, TAlignment> const& src )
{
	gs_array_derived<TSize, TAlignment> = src;
}

/// Vector

/// @brief: Class to simulate some type using heap memory.
struct Vector
{
	using range_t = std::vector<std::uint8_t>;

	Vector( std::size_t size = 0, std::uint8_t value = {} ) : m_data( size, value )
	{}
	Vector( range_t values ) : m_data{ std::move( values ) }
	{}

	auto begin() const
	{
		return m_data.begin();
	}
	auto begin()
	{
		return m_data.begin();
	}
	auto end() const
	{
		return m_data.end();
	}
	auto end()
	{
		return m_data.end();
	}

	static Vector sequence( std::size_t size, std::uint8_t offset = {} )
	{
		Vector ret( size, {} );
		std::iota( ret.begin(), ret.end(), offset );
		return ret;
	}

	operator range_t const &() const&
	{
		return m_data;
	};
	operator range_t&&() &&
	{
		return std::move( m_data );
	};
	operator range_t&() &
	{
		return m_data;
	};

	range_t m_data{};
};

bool operator==( Vector const& lhs, Vector const& rhs );
bool operator!=( Vector const& lhs, Vector const& rhs );

/// @brief: Track last call to call_user_api with some Vector
extern Vector gs_vector;
/// @brief: Track last call to call_user_api_derived with some Vector
extern Vector gs_vector_derived;

/// @brief: Allows creating UserApiModel with Vector
inline void call_user_api( Vector const& src )
{
	gs_vector = src;
}

/// @brief: Allows creating UserApiModelDerived with Vector
inline void call_user_api_derived( Vector const& src )
{
	gs_vector_derived = src;
}

} // namespace custom

template<typename T>
char const* type_name();

#include "custom.impl.hpp"

#endif
