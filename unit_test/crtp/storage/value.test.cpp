
#include "custom.hpp"

#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/value/value.hpp>
#include <crtp/storage/storage.hpp>

#include <gtest/gtest.h>

#include <any>
#include <string>

using custom::Array;
using custom::gs_array;
using custom::gs_vector;
using custom::UserValue;
using custom::UserValueBuilder;
using custom::UserValueConcept;
using custom::UserValueModel;
using custom::Vector;


namespace crtp::storage
{

template<typename T>
struct CrtpStorageValueT : public testing::Test
{
	using policy_t = T;
};

// clang-format off
using Test_types = ::testing::Types<
	OnHeap <UserValueBuilder>,
	OnStack<UserValueBuilder, 64>,
	OnStack<UserValueBuilder, 128>,
	OnStack<UserValueBuilder, 256>,
	Hybrid <UserValueBuilder, 4>,
	Hybrid <UserValueBuilder, 8>,
	Hybrid <UserValueBuilder, 16>,
	Hybrid <UserValueBuilder, 32>,
	Hybrid <UserValueBuilder, 64>,
	Hybrid <UserValueBuilder, 128>,
	Hybrid <UserValueBuilder, 256>
>;
// clang-format on

TYPED_TEST_SUITE( CrtpStorageValueT, Test_types );

TYPED_TEST( CrtpStorageValueT, ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserValue<policy_t> sot{ expected_vector };

	auto value = *sot.cast<Vector>();
	EXPECT_EQ( expected_vector, value );
}

TYPED_TEST( CrtpStorageValueT, copy_ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserValue<policy_t> const expected{ expected_vector };
	auto                      data = expected.cast<Vector>()->m_data.data();
	UserValue<policy_t>       sot{ expected };

	auto &original = *expected.cast<Vector>();
	EXPECT_EQ( expected_vector, original );
	auto &value = *sot.cast<Vector>();
	EXPECT_EQ( expected_vector, value );
	EXPECT_NE( data, value.m_data.data() );
}

TYPED_TEST( CrtpStorageValueT, copy_assign )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	Vector const other_vector{ 42, std::uint8_t{ 0 } };
	gs_vector = {};

	UserValue<policy_t> const expected{ expected_vector };
	auto data = expected.cast<Vector>()->m_data.data();
	UserValue<policy_t>       sot{ other_vector };

	sot       = expected;

	auto &original = *expected.cast<Vector>();
	EXPECT_EQ( expected_vector, original );
	auto &value = *sot.cast<Vector>();
	EXPECT_EQ( expected_vector, value );
	EXPECT_NE( data, value.m_data.data() );
}

TYPED_TEST( CrtpStorageValueT, move_ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	gs_vector = {};

	UserValue<policy_t> expected{ expected_vector };
	UserValue<policy_t> moved{ expected };

	auto                data = moved.cast<Vector>()->m_data.data();
	UserValue<policy_t> sot{ std::move( moved ) };

	auto &original = *expected.cast<Vector>();
	EXPECT_EQ( expected_vector, original );
	auto &moved_value = *moved.cast<Vector>();
	EXPECT_EQ( Vector{}, moved_value );
	auto &value = *sot.cast<Vector>();
	EXPECT_EQ( expected_vector, value );
	EXPECT_EQ( data, value.m_data.data() );
}

TYPED_TEST( CrtpStorageValueT, move_assign )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };
	Vector const other_vector{ 42, std::uint8_t{ 0 } };
	gs_vector = {};

	UserValue<policy_t> const expected{ expected_vector };
	UserValue<policy_t>       moved{ expected };
	UserValue<policy_t>       sot{ other_vector };

	auto data = moved.cast<Vector>()->m_data.data();
	sot = std::move( moved );

	auto &original = *expected.cast<Vector>();
	EXPECT_EQ( expected_vector, original );
	auto &moved_value = *moved.cast<Vector>();
	EXPECT_EQ( Vector{}, moved_value );
	auto &value = *sot.cast<Vector>();
	EXPECT_EQ( expected_vector, value );
	EXPECT_EQ( data, value.m_data.data() );
}
} // namespace crtp::storage
