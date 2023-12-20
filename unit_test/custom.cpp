#include "custom.hpp"

namespace custom
{
Vector gs_vector;

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
