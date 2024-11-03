#include "custom.hpp"

namespace custom
{
Vector gs_vector;
Vector gs_vector_derived;

/// Vector

bool operator==( Vector const& lhs, Vector const& rhs )
{
	return lhs.m_data == rhs.m_data;
}
} // namespace custom
