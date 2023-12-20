
#include "custom.hpp"

#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/storage.hpp>

#include <gtest/gtest.h>

#include <any>
#include <string>

using custom::Array;
using custom::gs_array;
using custom::gs_vector;
using custom::UserApi;
using custom::UserApiBuilder;
using custom::UserApiConcept;
using custom::UserApiModel;
using custom::Vector;


namespace crtp::storage
{
TEST( CrtpStorageUserApi, size )
{
	using array_t = Array<64>;
	EXPECT_EQ( 64, sizeof( array_t ) );
	using m_array_t = UserApiModel<array_t>;
	// @TechnicalDebt: 96, but why?
	EXPECT_LE( 64, sizeof( m_array_t ) );
	EXPECT_GE( 96, sizeof( m_array_t ) );
	using uac_t = UserApiConcept;
	EXPECT_GE( 8, sizeof( uac_t ) );
	using mcrtp_t = crtp::storage::Model<array_t, m_array_t, uac_t>;
	EXPECT_EQ( sizeof( m_array_t ), sizeof( mcrtp_t ) );
	EXPECT_EQ( 1, sizeof( crtp::Self<m_array_t, mcrtp_t> ) );
}

template<typename T>
struct CrtpStorageUserApiT : public testing::Test
{
	using policy_t = T;
};

// clang-format off
using Test_types = ::testing::Types<
	OnHeap <UserApiBuilder>,
	OnStack<UserApiBuilder, 64>,
	OnStack<UserApiBuilder, 128>,
	OnStack<UserApiBuilder, 256>,
	Hybrid <UserApiBuilder, 4>,
	Hybrid <UserApiBuilder, 8>,
	Hybrid <UserApiBuilder, 16>,
	Hybrid <UserApiBuilder, 32>,
	Hybrid <UserApiBuilder, 64>,
	Hybrid <UserApiBuilder, 128>,
	Hybrid <UserApiBuilder, 256>
>;
// clang-format on

TYPED_TEST_SUITE( CrtpStorageUserApiT, Test_types );

TYPED_TEST( CrtpStorageUserApiT, ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	gs_vector = {};

	UserApi<policy_t> sot{ expected_vector };

	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, copy_ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	gs_vector = {};

	UserApi<policy_t> const expected{ expected_vector };
	UserApi<policy_t>       sot{ expected };

	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, copy_assign )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	Vector const other_vector{ 42, std::uint8_t{ 0 } };
	gs_vector = {};

	UserApi<policy_t> const expected{ expected_vector };
	UserApi<policy_t>       sot{ other_vector };

	sot = expected;

	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, move_ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	gs_vector = {};

	UserApi<policy_t> expected{ Vector{ 1024, std::uint8_t{ 42 } } };
	UserApi<policy_t> moved{ expected };

	UserApi<policy_t> sot{ std::move( moved ) };

	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	moved.user_api();
	EXPECT_NE( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, move_assign )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	Vector const other_vector{ 42, std::uint8_t{ 0 } };
	gs_vector = {};

	UserApi<policy_t> const expected{ expected_vector };
	UserApi<policy_t>       moved{ expected };
	UserApi<policy_t>       sot{ other_vector };

	sot = std::move( moved );

	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	moved.user_api();
	EXPECT_NE( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}
} // namespace crtp::storage
