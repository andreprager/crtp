#pragma once

namespace crtp
{

// @brief:  Base class for CRTP interface generators providing convenience self() function.
// @tparam: TDerived   Derived class providing some basic interface (e.g. begin(), end())
// @tparam: TInterface Interface generator out of some basic interface (CRTP implementation)
template<typename TDerived, typename TInterface>
class Self
{
public:
	using self_t = TDerived;

	TDerived& self()
	{
		return static_cast<TDerived&>( *this );
	}
	TDerived const& self() const
	{
		return static_cast<TDerived const&>( *this );
	}

private:
	Self()
	{}
	friend TInterface;
	friend TDerived;
};

} // namespace crtp
