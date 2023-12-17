#include "custom.hpp"

namespace custom
{
/// Type

std::ostream& operator<<( std::ostream& os, Type const& value )
{
	return os << value.m_value;
}

bool operator==( Type const& lhs, Type const& rhs )
{
	return lhs.m_value == rhs.m_value;
}

bool operator!=( Type const& lhs, Type const& rhs )
{
	return !( lhs == rhs );
}

/// String

std::ostream& operator<<( std::ostream& os, String const& value )
{
	return os << value.m_value;
}

bool operator==( String const& lhs, String const& rhs )
{
	return lhs.m_value == rhs.m_value;
}

bool operator!=( String const& lhs, String const& rhs )
{
	return !( lhs == rhs );
}

/// Vector

bool operator==( Vector const& lhs, Vector const& rhs )
{
	return lhs.m_data == rhs.m_data;
}

bool operator!=( Vector const& lhs, Vector const& rhs )
{
	return !( lhs == rhs );
}
} // namespace custom
