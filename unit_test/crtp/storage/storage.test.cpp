
#include "custom.hpp"

#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/storage.hpp>

#include <gtest/gtest.h>

#include <any>
#include <string>
#include <utility>

using custom::Array;
using custom::gs_array;
using custom::gs_vector;
using custom::gs_vector_derived;
using custom::UserApi;
using custom::UserApiBuilder;
using custom::UserApiBuilderDerived;
using custom::UserApiConcept;
using custom::UserApiConceptDerived;
using custom::UserApiDerived;
using custom::UserApiModel;
using custom::UserApiModelDerived;
using custom::Vector;


struct InvalidBuilder
{};

struct InvalidBuilderHeap
{
	using memory_tag_t = int;
	using concept_t    = int;
	using clone_t      = std::unique_ptr<int>;
	void destroy( concept_t* )
	{}
};

namespace crtp::storage
{

TEST( CrtpStorageUserApi, storage_size )
{
	using uac_t = UserApiConcept;
	EXPECT_EQ( sizeof( void* ), sizeof( uac_t ) );
	using uab_t = UserApiBuilder;
	EXPECT_EQ( 1, sizeof( uab_t ) );
	using oh_t = OnHeap<UserApiBuilder>;
	EXPECT_EQ( 8, sizeof( oh_t ) );
	using os_t = OnStack<UserApiBuilder, 80>;
	EXPECT_EQ( 80, sizeof( os_t ) );
	using hy_t = Hybrid<UserApiBuilder, 80>;
	EXPECT_EQ( 96, sizeof( hy_t ) );

	// error: missing internal types in builder
	// using invalid_t  = OnHeap<InvalidBuilder>;
	using invalid_heap_t = OnHeap<InvalidBuilderHeap>;
	// error: no static @build(...) function
	// invalid_heap_t invalid{ 42 };
}

TEST( CrtpStorageUserApi, storage_size_derived )
{
	using uac_t = UserApiConceptDerived;
	EXPECT_EQ( 2 * sizeof( void* ), sizeof( uac_t ) );
	using uab_t = UserApiBuilderDerived;
	EXPECT_EQ( 1, sizeof( uab_t ) );
	using oh_t = OnHeap<UserApiBuilderDerived>;
	EXPECT_EQ( 8, sizeof( oh_t ) );
	using os_t = OnStack<UserApiBuilderDerived, 80>;
	EXPECT_EQ( 80, sizeof( os_t ) );
	using hy_t = Hybrid<UserApiBuilderDerived, 80>;
	EXPECT_EQ( 96, sizeof( hy_t ) );

	// error: missing internal types in builder
	// using invalid_t  = OnHeap<InvalidBuilder>;
	using invalid_heap_t = OnHeap<InvalidBuilderHeap>;
	// error: no static @build(...) function
	// invalid_heap_t invalid{ 42 };
}

TEST( CrtpStorageUserApi, array_size )
{
	using sot_t = Array<64>;
	EXPECT_EQ( 64, sizeof( sot_t ) );
	using uac_t = UserApiConcept;
	using uam_t = UserApiModel<sot_t>;
	// @TechnicalDebt: 80, but why so big?
	EXPECT_LE( 64, sizeof( uam_t ) );
	EXPECT_LE( sizeof( uam_t ), 96 );
	using m_sot_t = crtp::storage::Model<sot_t, uam_t, uac_t>;
	EXPECT_EQ( sizeof( uam_t ), sizeof( m_sot_t ) );
	EXPECT_EQ( 1, sizeof( crtp::Self<uam_t, m_sot_t> ) );
}

TEST( CrtpStorageUserApi, vector_size )
{
	using sot_t = Vector;
#ifndef NDEBUG
	EXPECT_EQ( 32, sizeof( sot_t ) );
#else
	EXPECT_EQ( 24, sizeof( sot_t ) );
#endif
	using uac_t = UserApiConcept;
	using uam_t = UserApiModel<sot_t>;
	// @TechnicalDebt: 40, but why so big?
	EXPECT_LE( 32, sizeof( uam_t ) );
	EXPECT_GE( 64, sizeof( uam_t ) );
	using m_sot_t = crtp::storage::Model<sot_t, uam_t, uac_t>;
	EXPECT_EQ( sizeof( uam_t ), sizeof( m_sot_t ) );
	EXPECT_EQ( 1, sizeof( crtp::Self<uam_t, m_sot_t> ) );
}

template<typename T>
struct CrtpStorageUserApiT : public testing::Test
{
	using policy_t = T;
};

// clang-format off
using Test_types = ::testing::Types<
	OnHeap <UserApiBuilder>,
	OnStack<UserApiBuilder, 80>,
	OnStack<UserApiBuilder, 128>,
	OnStack<UserApiBuilder, 256>,
	Hybrid <UserApiBuilder, 4>,
	Hybrid <UserApiBuilder, 8>,
	Hybrid <UserApiBuilder, 16>,
	Hybrid <UserApiBuilder, 32>,
	Hybrid <UserApiBuilder, 64>,
	Hybrid <UserApiBuilder, 80>,
	Hybrid <UserApiBuilder, 128>,
	Hybrid <UserApiBuilder, 256>
>;
// clang-format on

TYPED_TEST_SUITE( CrtpStorageUserApiT, Test_types );

TYPED_TEST( CrtpStorageUserApiT, ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserApi<policy_t> sot{ expected_vector };

	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, copy_ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const            expected_vector{ 1024, std::uint8_t{ 42 } };
	UserApi<policy_t> const expected{ expected_vector };

	UserApi<policy_t> sot{ expected };

	gs_vector = {};
	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, copy_assign )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const            expected_vector{ 1024, std::uint8_t{ 42 } };
	Vector const            other_vector{ 42, std::uint8_t{ 0 } };
	UserApi<policy_t> const expected{ expected_vector };

	UserApi<policy_t> sot{ other_vector };

	sot = expected;

	gs_vector = {};
	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, copy_assign_self )
{
	using policy_t = typename TestFixture::policy_t;

	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserApi<policy_t> sot{ expected_vector };

	sot = sot;

	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, move_ctor )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const      empty_vector{};
	Vector const      expected_vector{ 1024, std::uint8_t{ 42 } };
	UserApi<policy_t> moved{ expected_vector };

	UserApi<policy_t> sot{ std::move( moved ) };

	gs_vector = {};
	moved.user_api();
	EXPECT_EQ( empty_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, move_assign )
{
	using policy_t = typename TestFixture::policy_t;
	Vector const      empty_vector{};
	Vector const      expected_vector{ 1024, std::uint8_t{ 42 } };
	Vector const      other_vector{ 42, std::uint8_t{ 0 } };
	UserApi<policy_t> moved{ expected_vector };

	UserApi<policy_t> sot{ other_vector };
	sot = std::move( moved );

	gs_vector = {};
	moved.user_api();
	EXPECT_EQ( empty_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiT, move_assign_self )
{
	using policy_t = typename TestFixture::policy_t;

	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserApi<policy_t> sot{ expected_vector };

	sot = std::move( sot );

	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

template<typename T>
struct CrtpStorageUserApiMixedT : public testing::Test
{
	using policy_lsh_t = typename T::first_type;
	using policy_rsh_t = typename T::second_type;
};

// clang-format off
using TestMixed_types = ::testing::Types<
	// same family but different sizes
	std::pair<OnStack<UserApiBuilder, 80>,  OnStack<UserApiBuilder, 80>>,
	std::pair<OnStack<UserApiBuilder, 80>,  OnStack<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 80>,  OnStack<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilder, 80>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilder, 80>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilder, 256>>,
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
	// mixed families
	std::pair<OnStack<UserApiBuilder, 80>, Hybrid<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 80>, Hybrid<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 80>, Hybrid<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 128>, Hybrid<UserApiBuilder, 256>>,
	std::pair<OnStack<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 64>>,
	std::pair<OnStack<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 128>>,
	std::pair<OnStack<UserApiBuilder, 256>, Hybrid<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 64>, OnStack<UserApiBuilder, 80>>,
	std::pair<Hybrid<UserApiBuilder, 64>, OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 64>, OnStack<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnStack<UserApiBuilder, 80>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 128>, OnStack<UserApiBuilder, 256>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnStack<UserApiBuilder, 80>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnStack<UserApiBuilder, 128>>,
	std::pair<Hybrid<UserApiBuilder, 256>, OnStack<UserApiBuilder, 256>>
>;
// clang-format on

TYPED_TEST_SUITE( CrtpStorageUserApiMixedT, TestMixed_types );

TYPED_TEST( CrtpStorageUserApiMixedT, copy_ctor )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	Vector const          expected_vector{ 1024, std::uint8_t{ 42 } };
	UserApi<policy_rsh_t> expected{ expected_vector };

	UserApi<policy_lsh_t> sot{ expected };

	gs_vector = {};
	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiMixedT, copy_assign )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	Vector const          other_vector{ 1024, std::uint8_t{ 13 } };
	Vector const          expected_vector{ 1024, std::uint8_t{ 42 } };
	UserApi<policy_rsh_t> expected{ expected_vector };

	UserApi<policy_lsh_t> sot{ other_vector };
	sot = expected;

	gs_vector = {};
	expected.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiMixedT, move_ctor )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	Vector const empty_vector{};
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserApi<policy_lsh_t> moved{ expected_vector };

	UserApi<policy_rsh_t> sot{ std::move( moved ) };

	gs_vector = {};
	moved.user_api();
	EXPECT_EQ( empty_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TYPED_TEST( CrtpStorageUserApiMixedT, move_assign )
{
	using policy_lsh_t = typename TestFixture::policy_lsh_t;
	using policy_rsh_t = typename TestFixture::policy_rsh_t;

	Vector const          empty_vector{};
	Vector const          other_vector{ 1024, std::uint8_t{ 13 } };
	Vector const          expected_vector{ 1024, std::uint8_t{ 42 } };
	UserApi<policy_rsh_t> moved{ expected_vector };

	UserApi<policy_lsh_t> sot{ other_vector };
	sot = std::move( moved );

	gs_vector = {};
	moved.user_api();
	EXPECT_EQ( empty_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

template<typename T>
struct CrtpStorageUserApiAssignT : public testing::Test
{
	using expected_t   = std::tuple_element_t<0, T>;
	using policy_lsh_t = std::tuple_element_t<1, T>;
	using policy_rsh_t = std::tuple_element_t<2, T>;
};

// clang-format off
using TestAssign_types = ::testing::Types<
	// same policy but different sizes
	std::tuple<std::true_type, OnStack<UserApiBuilder,  64>, OnStack<UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder,  64>, OnStack<UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder,  64>, OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 128>, OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 256>, OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, OnHeap<UserApiBuilder>,       OnHeap< UserApiBuilderDerived>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  32>,  Hybrid< UserApiBuilderDerived,  32>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  32>,  Hybrid< UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  32>,  Hybrid< UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  32>,  Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  64>,  Hybrid< UserApiBuilderDerived,  32>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  64>,  Hybrid< UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  64>,  Hybrid< UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder,  64>,  Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 128>,  Hybrid< UserApiBuilderDerived,  32>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 128>,  Hybrid< UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 128>,  Hybrid< UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 128>,  Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 256>,  Hybrid< UserApiBuilderDerived,  32>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 256>,  Hybrid< UserApiBuilderDerived,  64>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 256>,  Hybrid< UserApiBuilderDerived, 128>>,
	std::tuple<std::true_type, Hybrid<UserApiBuilder, 256>,  Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived,  64>, OnStack<UserApiBuilder,  64>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived,  64>, OnStack<UserApiBuilder, 128>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived,  64>, OnStack<UserApiBuilder, 256>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 128>, OnStack<UserApiBuilder,  64>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 128>, OnStack<UserApiBuilder, 128>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 128>, OnStack<UserApiBuilder, 256>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 256>, OnStack<UserApiBuilder,  64>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 256>, OnStack<UserApiBuilder, 128>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 256>, OnStack<UserApiBuilder, 256>>,
	std::tuple<std::false_type, OnHeap< UserApiBuilderDerived>,      OnHeap< UserApiBuilder>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  32>, Hybrid< UserApiBuilder,  32>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  32>, Hybrid< UserApiBuilder,  64>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  32>, Hybrid< UserApiBuilder, 128>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  32>, Hybrid< UserApiBuilder, 256>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, Hybrid< UserApiBuilder,  32>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, Hybrid< UserApiBuilder,  64>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, Hybrid< UserApiBuilder, 128>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, Hybrid< UserApiBuilder, 256>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 128>, Hybrid< UserApiBuilder,  32>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 128>, Hybrid< UserApiBuilder,  64>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 128>, Hybrid< UserApiBuilder, 128>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 128>, Hybrid< UserApiBuilder, 256>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, Hybrid< UserApiBuilder,  32>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, Hybrid< UserApiBuilder,  64>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, Hybrid< UserApiBuilder, 128>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, Hybrid< UserApiBuilder, 256>>,
	// mixed policies
	std::tuple<std::true_type, OnStack<UserApiBuilder, 64>,  OnHeap< UserApiBuilderDerived>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 256>, OnHeap< UserApiBuilderDerived>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 64>,  Hybrid< UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 64>,  Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 256>, Hybrid< UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, OnStack<UserApiBuilder, 256>, Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, OnHeap< UserApiBuilder>,      OnStack<UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, OnHeap< UserApiBuilder>,      OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, OnHeap< UserApiBuilder>,      Hybrid< UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, OnHeap< UserApiBuilder>,      Hybrid< UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder,  64>, OnHeap< UserApiBuilderDerived>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder, 256>, OnHeap< UserApiBuilderDerived>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder,  64>, OnStack<UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder,  64>, OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder,  64>, OnStack<UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder,  64>, OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder, 256>, OnStack<UserApiBuilderDerived, 64>>,
	std::tuple<std::true_type, Hybrid< UserApiBuilder, 256>, OnStack<UserApiBuilderDerived, 256>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 64>,  OnHeap< UserApiBuilder>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 256>, OnHeap< UserApiBuilder>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 64>,  Hybrid< UserApiBuilder, 64>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 64>,  Hybrid< UserApiBuilder, 256>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 256>, Hybrid< UserApiBuilder, 64>>,
	std::tuple<std::false_type, OnStack<UserApiBuilderDerived, 256>, Hybrid< UserApiBuilder, 256>>,
	std::tuple<std::false_type, OnHeap< UserApiBuilderDerived>,      OnStack<UserApiBuilder, 64>>,
	std::tuple<std::false_type, OnHeap< UserApiBuilderDerived>,      OnStack<UserApiBuilder, 256>>,
	std::tuple<std::false_type, OnHeap< UserApiBuilderDerived>,      Hybrid< UserApiBuilder, 64>>,
	std::tuple<std::false_type, OnHeap< UserApiBuilderDerived>,      Hybrid< UserApiBuilder, 256>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, OnHeap< UserApiBuilder>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, OnHeap< UserApiBuilder>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, OnStack<UserApiBuilder, 64>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived,  64>, OnStack<UserApiBuilder, 256>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, OnStack<UserApiBuilder, 64>>,
	std::tuple<std::false_type, Hybrid< UserApiBuilderDerived, 256>, OnStack<UserApiBuilder, 256>>
>;
// clang-format on

TYPED_TEST_SUITE( CrtpStorageUserApiAssignT, TestAssign_types );

TYPED_TEST( CrtpStorageUserApiAssignT, traits )
{
	using expected_t   = typename TestFixture::expected_t;
	using policy_dst_t = typename TestFixture::policy_lsh_t;
	using policy_src_t = typename TestFixture::policy_rsh_t;

	// static_assert( expected_t::value == crtp::storage::traits::PolicyAssign<policy_src_t, policy_dst_t> );
	EXPECT_EQ( expected_t::value, ( crtp::storage::traits::PolicyAssign<policy_src_t, policy_dst_t> ) );
}

TEST( CrtpStorageUserApiDerived, assign )
{
	using policy_lsh_t = Hybrid<UserApiBuilder, 256>;
	using policy_rsh_t = Hybrid<UserApiBuilderDerived, 256>;

	Vector const                 empty_vector{};
	Vector const                 other_vector{ 1024, std::uint8_t{ 13 } };
	Vector const                 expected_vector{ 1024, std::uint8_t{ 42 } };
	UserApiDerived<policy_rsh_t> moved_rsh{ expected_vector };
	UserApiDerived<policy_lsh_t> moved_lsh{ expected_vector };
	UserApi<policy_lsh_t>        sot{ other_vector };

	sot = moved_rsh;
}

TEST( CrtpStorageUserApiDerived, ctor )
{
	Vector const empty_vector{};
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	std::array<std::byte, 256>   buffer{};
	std::array<std::byte, 256>   buffer_derived{};
	UserApiConcept* const        memory{ reinterpret_cast<UserApiConcept*>( buffer.data() ) };
	UserApiConceptDerived* const memory_derived{ reinterpret_cast<UserApiConceptDerived*>( buffer_derived.data() ) };
	auto                         model         = reinterpret_cast<UserApiModelDerived<Vector>*>( memory );
	auto                         model_derived = reinterpret_cast<UserApiModelDerived<Vector>*>( memory_derived );
	std::construct_at( model, expected_vector );
	std::construct_at( model_derived, expected_vector );

	gs_vector = {};
	model->user_api();
	EXPECT_EQ( expected_vector, gs_vector );

	gs_vector = {};
	model_derived->user_api();
	EXPECT_EQ( expected_vector, gs_vector );

	gs_vector_derived = {};
	model->user_api_derived();
	EXPECT_EQ( expected_vector, gs_vector_derived );

	gs_vector_derived = {};
	model_derived->user_api_derived();
	EXPECT_EQ( expected_vector, gs_vector_derived );

	std::destroy_at( model_derived );
	std::destroy_at( model );
}

TEST( CrtpStorageUserApiDerived, copy_assign )
{
	using policy_lsh_t = OnStack<UserApiBuilder, 128>;
	using policy_rsh_t = OnStack<UserApiBuilderDerived, 128>;

	Vector const empty_vector{};
	Vector const other_vector{ 1024, std::uint8_t{ 13 } };
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserApiDerived<policy_rsh_t> cloned{ expected_vector };
	UserApi<policy_lsh_t>        sot{ other_vector };
	sot = cloned;

	gs_vector = {};
	cloned.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
	gs_vector_derived = {};
	cloned.user_api_derived();
	EXPECT_EQ( expected_vector, gs_vector_derived );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

TEST( CrtpStorageUserApiDerived, move_assign )
{
	using policy_lsh_t = OnStack<UserApiBuilder, 128>;
	using policy_rsh_t = OnStack<UserApiBuilderDerived, 128>;

	Vector const empty_vector{};
	Vector const other_vector{ 1024, std::uint8_t{ 13 } };
	Vector const expected_vector{ 1024, std::uint8_t{ 42 } };

	UserApi<policy_rsh_t> moved{ expected_vector };
	UserApi<policy_lsh_t> sot{ other_vector };
	sot = std::move( moved );

	gs_vector = {};
	moved.user_api();
	EXPECT_EQ( empty_vector, gs_vector );
	gs_vector = {};
	sot.user_api();
	EXPECT_EQ( expected_vector, gs_vector );
}

} // namespace crtp::storage
