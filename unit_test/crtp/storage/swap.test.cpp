
#include "custom.hpp"

#include <crtp/storage/builder/builder.hpp>
#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/storage.hpp>
#include <crtp/storage/swap/swap.hpp>

#include <gtest/gtest.h>

#include <any>
#include <string>

using custom::Array;
using custom::call_user_api;
using custom::gs_array;
using custom::gs_vector;
using custom::UserApi;
using custom::UserApiBuilder;
using custom::UserApiConcept;
using custom::UserApiModel;
using custom::Vector;


namespace crtp::storage
{

TEST( CrtpStorageSwap, swap_Vector )
{
	UserApi<OnHeap<UserApiBuilder>>      sot0{ Vector::sequence( 256 ) };
	UserApi<OnStack<UserApiBuilder, 64>> sot1{ Vector::sequence( 256, 42 ) };

	call_user_api( sot0 );
	{
		EXPECT_EQ( 256, gs_vector.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	call_user_api( sot1 );
	{
		EXPECT_EQ( 256, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}

	swap( sot0, sot1 );

	call_user_api( sot0 );
	{
		EXPECT_EQ( 256, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	call_user_api( sot1 );
	{
		EXPECT_EQ( 256, gs_vector.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
}

TEST( CrtpStorageSwap, swap_Array )
{
	UserApi<OnHeap<UserApiBuilder>>      sot0{ Array<64>::sequence( 256 ) };
	UserApi<OnStack<UserApiBuilder, 96>> sot1{ Array<64>::sequence( 256, 42 ) };

	call_user_api( sot0 );
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	call_user_api( sot1 );
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}

	swap( sot0, sot1 );

	call_user_api( sot0 );
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	call_user_api( sot1 );
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
}

TEST( CrtpStorageSwap, swap_Stack_Vector_Heap_Array )
{
	UserApi<OnHeap<UserApiBuilder>>      sot0{ Array<64>::sequence( 256 ) };
	UserApi<OnStack<UserApiBuilder, 96>> sot1{ Vector::sequence( 64, 42 ) };

	call_user_api( sot0 );
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	call_user_api( sot1 );
	{
		EXPECT_EQ( 64, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}

	swap( sot0, sot1 );

	gs_array<64> = {};
	gs_vector    = {};

	call_user_api( sot0 );
	{
		EXPECT_EQ( 64, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index, entry );
		}
	}

	call_user_api( sot1 );
	{
		EXPECT_EQ( 64, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
}

TEST( CrtpStorageSwap, swap_Stack_Array_Heap_Vector )
{
	UserApi<OnStack<UserApiBuilder, 96>> sot0{ Array<64>::sequence( 256 ) };
	UserApi<OnHeap<UserApiBuilder>>      sot1{ Vector::sequence( 64, 42 ) };

	call_user_api( sot0 );
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	call_user_api( sot1 );
	{
		EXPECT_EQ( 64, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}

	swap( sot0, sot1 );

	gs_array<64> = {};
	gs_vector    = {};

	call_user_api( sot0 );
	{
		EXPECT_EQ( 64, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index, entry );
		}
	}

	call_user_api( sot1 );
	{
		EXPECT_EQ( 64, gs_vector.m_data.size() );
		std::uint8_t index{ 42 };
		for ( auto& entry : gs_vector )
		{
			EXPECT_EQ( index++, entry );
		}
	}
	{
		EXPECT_EQ( 64, gs_array<64>.m_data.size() );
		std::uint8_t index{};
		for ( auto& entry : gs_array<64> )
		{
			EXPECT_EQ( index++, entry );
		}
	}
}

} // namespace crtp::storage
