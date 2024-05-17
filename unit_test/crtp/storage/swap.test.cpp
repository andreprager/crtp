
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
template<typename T>
struct CrtpStorageSwapT : public testing::Test
{
	using policy_lsh_t = typename T::first_type;
	using policy_rsh_t = typename T::second_type;
};

// clang-format off
using TestSwap_types = ::testing::Types<
	// OnStack - OnStack
	std::pair<OnStack<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilder, 256>>,
	// OnStack - Hybrid
	std::pair<OnStack<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  OnStack<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnStack<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnStack<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnStack<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnStack<UserApiBuilder, 256>>,
	// OnStack - OnHeap
	std::pair<OnStack<UserApiBuilder, 64>,  OnHeap<UserApiBuilder>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnHeap<UserApiBuilder>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnHeap<UserApiBuilder>>,
	std::pair<OnHeap<UserApiBuilder>, OnStack<UserApiBuilder, 64>>,
	std::pair<OnHeap<UserApiBuilder>, OnStack<UserApiBuilder, 128>>,
	std::pair<OnHeap<UserApiBuilder>, OnStack<UserApiBuilder, 256>>,
	// Hybrid - Hybrid
	std::pair<Hybrid<UserApiBuilder, 32>,  Hybrid<UserApiBuilder, 32>>,
	std::pair<Hybrid<UserApiBuilder, 32>,  Hybrid<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 32>,  Hybrid<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 32>,  Hybrid<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 32>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 64>,  Hybrid<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 32>>,
	std::pair<Hybrid<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 32>>,
	std::pair<Hybrid<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 64>>,
	std::pair<Hybrid<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 256>>,
	// Hybrid - OnHeap
	std::pair<Hybrid<UserApiBuilder, 64>,  OnHeap<UserApiBuilder>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnHeap<UserApiBuilder>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnHeap<UserApiBuilder>>,
	std::pair<OnHeap<UserApiBuilder>, Hybrid<UserApiBuilder, 64>>,
	std::pair<OnHeap<UserApiBuilder>, Hybrid<UserApiBuilder, 128>>,
	std::pair<OnHeap<UserApiBuilder>, Hybrid<UserApiBuilder, 256>>,
	// OnHeap - OnHeap
	std::pair<OnHeap<UserApiBuilder>,  OnHeap<UserApiBuilder>>
>;
// clang-format on

TYPED_TEST_SUITE( CrtpStorageSwapT, TestSwap_types );

TYPED_TEST( CrtpStorageSwapT, swap_Vector )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	Vector const vector0{ Vector::sequence( 256 ) };
	Vector const vector1{ Vector::sequence( 256, 42 ) };

	UserApi<policy_lsh_t> sot0{ vector0 };
	UserApi<policy_rsh_t> sot1{ vector1 };

	gs_vector = {};
	call_user_api( sot0 );
	EXPECT_EQ( vector0, gs_vector );

	gs_vector = {};
	call_user_api( sot1 );
	EXPECT_EQ( vector1, gs_vector );

	swap( sot0, sot1 );

	gs_vector = {};
	call_user_api( sot0 );
	EXPECT_EQ( vector1, gs_vector );

	gs_vector = {};
	call_user_api( sot1 );
	EXPECT_EQ( vector0, gs_vector );
}

TYPED_TEST( CrtpStorageSwapT, swap_Array )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	constexpr std::size_t Size = 32;

	auto const vector0{ Array<Size>::sequence( Size ) };
	auto const vector1{ Array<Size>::sequence( Size, 42 ) };

	UserApi<policy_lsh_t> sot0{ vector0 };
	UserApi<policy_rsh_t> sot1{ vector1 };

	gs_array<Size> = {};
	call_user_api( sot0 );
	EXPECT_EQ( vector0, gs_array<Size> );

	gs_array<Size> = {};
	call_user_api( sot1 );
	EXPECT_EQ( vector1, gs_array<Size> );

	swap( sot0, sot1 );

	gs_array<Size> = {};
	call_user_api( sot0 );
	EXPECT_EQ( vector1, gs_array<Size> );

	gs_array<Size> = {};
	call_user_api( sot1 );
	EXPECT_EQ( vector0, gs_array<Size> );
}

TYPED_TEST( CrtpStorageSwapT, swap_Vector_Array )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	constexpr std::size_t Size = 32;

	auto const v{ Vector::sequence( 4 * Size ) };
	auto const a{ Array<Size>::sequence( Size, 42 ) };

	UserApi<policy_lsh_t> sot0{ v };
	UserApi<policy_rsh_t> sot1{ a };

	gs_vector    = {};
	gs_array<Size> = {};
	call_user_api( sot0 );
	EXPECT_EQ( v, gs_vector );
	EXPECT_EQ( Array<Size>{}, gs_array<Size>);

	gs_array<Size> = {};
	call_user_api( sot1 );
	EXPECT_EQ( v, gs_vector );
	EXPECT_EQ( a, gs_array<Size> );

	// sot0 -> v
	// sot1 -> a

	swap( sot0, sot1 );
	// sot0 -> a
	// sot1 -> v

	gs_array<Size> = {};
	call_user_api( sot0 );
	EXPECT_EQ( v, gs_vector );
	EXPECT_EQ( a, gs_array<Size> );

	gs_vector    = {};
	call_user_api( sot1 );
	EXPECT_EQ( v, gs_vector );
	EXPECT_EQ( a, gs_array<Size> );

	swap( sot0, sot1 );
	// sot0 -> v
	// sot1 -> a

	gs_array<Size> = {};
	gs_vector    = {};
	call_user_api( sot0 );
	EXPECT_EQ( v, gs_vector );
	EXPECT_EQ( Array<Size>{}, gs_array<Size>);

	gs_array<Size> = {};
	call_user_api( sot1 );
	EXPECT_EQ( v, gs_vector );
	EXPECT_EQ( a, gs_array<Size> );
}

} // namespace crtp::storage
