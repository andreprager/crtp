#ifndef utility_custom_impl_hpp
#define utility_custom_impl_hpp

#include "custom.hpp"

namespace custom
{
/// Array

template<std::size_t TSize, std::size_t TAlignment>
inline bool operator==( Array<TSize, TAlignment> const& lhs, Array<TSize, TAlignment> const& rhs )
{
	return lhs.m_data == rhs.m_data;
}

template<std::size_t TSize, std::size_t TAlignment>
inline bool operator!=( Array<TSize, TAlignment> const& lhs, Array<TSize, TAlignment> const& rhs )
{
	return !( lhs == rhs );
}

} // namespace custom

template<typename T>
inline char const* type_name()
{
	return typeid( T ).name();
}


#endif
