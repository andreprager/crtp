#ifndef utility_custom_hpp
#define utility_custom_hpp

#include <crtp/self.hpp>
#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/storage.hpp>

#include <array>
#include <memory>
#include <numeric>
#include <ostream>
#include <string>
#include <vector>

namespace custom
{
/// Type

struct Type
{
	int m_value;
};

bool          operator==( Type const& lhs, Type const& rhs );
bool          operator!=( Type const& lhs, Type const& rhs );
std::ostream& operator<<( std::ostream& os, Type const& value );

/// String

struct String
{
	std::string m_value;
};

bool          operator==( String const& lhs, String const& rhs );
bool          operator!=( String const& lhs, String const& rhs );
std::ostream& operator<<( std::ostream& os, String const& value );

/// Array

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

	operator range_t const&() const
	{
		return m_data;
	};
	operator range_t&()
	{
		return m_data;
	};

	alignas( TAlignment ) range_t m_data;
};

template<std::size_t TSize = 128, std::size_t TAlignment = 16>
bool operator==( Array<TSize, TAlignment> const& lhs, Array<TSize, TAlignment> const& rhs );
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
bool operator!=( Array<TSize, TAlignment> const& lhs, Array<TSize, TAlignment> const& rhs );

template<std::size_t TSize = 128, std::size_t TAlignment = 16>
Array<TSize, TAlignment> gs_array;

/// @brief: Allows creating UserApiModel with Array
template<std::size_t TSize = 128, std::size_t TAlignment = 16>
inline void call_user_api( Array<TSize, TAlignment> const& src )
{
	gs_array<TSize, TAlignment> = src;
}

/// Vector

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

	operator range_t const&() const
	{
		return m_data;
	};
	operator range_t&()
	{
		return m_data;
	};

	range_t m_data{};
};

bool operator==( Vector const& lhs, Vector const& rhs );
bool operator!=( Vector const& lhs, Vector const& rhs );

extern Vector gs_vector;

/// @brief: Allows creating UserApiModel with Vector
inline void call_user_api( Vector const& src )
{
	gs_vector = src;
}

// UserApiConcept
// @customize: Define interface once
class UserApiConcept : public crtp::storage::detail::Concept<UserApiConcept>
{
public:
	using concept_t = crtp::storage::detail::Concept<UserApiConcept>;
	using typename concept_t::clone_t;

	/// @brief: customizable interface functions
	/// @customize
	virtual void user_api() const = 0;
};

inline void call_user_api( UserApiConcept const& src )
{
	src.user_api();
}

/// UserApiModel

/// @brief: Automatically create UserApiModel for some value of Type T.
/// @pre:   Function call_user_api(T,...) must exist.
// @customize: Define model once, implementing interface
template<typename T>
class UserApiModel : public crtp::storage::detail::Model<T, UserApiModel<T>, UserApiConcept>
{
public:
	using model_t = crtp::storage::detail::Model<T, UserApiModel<T>, UserApiConcept>;
	UserApiModel( T value ) : model_t{ std::move( value ) }
	{}

	/// @customize
	void user_api() const override
	{
		call_user_api( this->m_value );
	}
};

template<typename TStoragePolicy>
class UserApi : public crtp::storage::Storage<UserApi<TStoragePolicy>, TStoragePolicy>
{
public:
	using storage_t = crtp::storage::Storage<UserApi<TStoragePolicy>, TStoragePolicy>;

	/// @brief:
	/// @pre:   call_user_api function
	template<typename T>
	UserApi( T value ) : storage_t{ std::move( value ) }
	{}

	void user_api() const
	{
		this->m_storage.memory()->user_api();
	}

	template<typename T>
	friend void call_user_api( UserApi<T> const& src );
};

template<typename T>
void call_user_api( UserApi<T> const& src )
{
	src.user_api();
}

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

} // namespace custom

template<typename T>
char const* type_name();

#include "custom.impl.hpp"

#endif
