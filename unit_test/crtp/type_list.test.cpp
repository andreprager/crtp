#include <crtp/type_list.hpp>

#include <gtest/gtest.h>

#include <custom.hpp>

#include <any>
#include <string>
#include <type_traits>

namespace crtp
{
namespace
{
template<typename... Ts>
struct TypeListConfig
{
	using type_list_t = TypeList<Ts...>;
	using tuple_t     = std::tuple<Ts...>;

	static constexpr std::size_t const msc_size = std::tuple_size_v<tuple_t>;
};

// clang-format off
using Tuple_types = ::testing::Types<
	TypeListConfig<int>,
	TypeListConfig<int, float>,
	TypeListConfig<custom::Vector, int>,
	TypeListConfig<custom::Array<64>, std::string>,
	TypeListConfig<custom::Array<128, 1>, std::array<int, 128>>,
	TypeListConfig<>
>;
// clang-format on
} // namespace

template<typename T>
struct CrtpTypeListT : public testing::Test
{
	static constexpr std::size_t const msc_size = T::msc_size;

	using type_list_t = typename T::type_list_t;
	using tuple_t     = typename T::tuple_t;
};

TYPED_TEST_SUITE( CrtpTypeListT, Tuple_types );

TYPED_TEST( CrtpTypeListT, is_type_list )
{
	using type_list_t = typename TestFixture::type_list_t;
	using tuple_t     = typename TestFixture::tuple_t;

	EXPECT_TRUE( IsTypeList_v<type_list_t> );
	EXPECT_FALSE( IsTypeList_v<tuple_t> );
	EXPECT_FALSE( IsTypeList_v<void> );
	EXPECT_FALSE( IsTypeList_v<int> );

	[]( type_list auto ) {}( type_list_t{} );
}

TYPED_TEST( CrtpTypeListT, make_tuple )
{
	using type_list_t = typename TestFixture::type_list_t;
	using tuple_t     = typename TestFixture::tuple_t;

	using sot_t = MakeTuple_t<type_list_t>;

	EXPECT_TRUE( ( std::is_same_v<tuple_t, sot_t> ) );
}

TYPED_TEST( CrtpTypeListT, make_type_list )
{
	using type_list_t = typename TestFixture::type_list_t;
	using tuple_t     = typename TestFixture::tuple_t;

	constexpr auto const expected_size = TestFixture::msc_size;

	using sot_t = MakeTypeList_t<tuple_t>;

	EXPECT_TRUE( ( std::is_same_v<type_list_t, sot_t> ) );
}

TYPED_TEST( CrtpTypeListT, size )
{
	using type_list_t = typename TestFixture::type_list_t;
	using tuple_t     = typename TestFixture::tuple_t;

	constexpr auto const expected_size = TestFixture::msc_size;

	EXPECT_EQ( expected_size, TypeList_size<type_list_t> );
	EXPECT_EQ( expected_size, TypeListSize_v<type_list_t> );
}

TYPED_TEST( CrtpTypeListT, index_valid )
{
	using type_list_t = typename TestFixture::type_list_t;
	using tuple_t     = typename TestFixture::tuple_t;

	constexpr auto const expected_size = TestFixture::msc_size;

	EXPECT_FALSE( ( IsTypeIndexValid_v<-1, type_list_t> ) );
	constexpr bool const has_0 = 0 < expected_size;
	EXPECT_EQ( has_0, ( IsTypeIndexValid_v<0, type_list_t> ) );
	EXPECT_EQ( has_0, ( IsTypeIndexValid_v<expected_size / 2, type_list_t> ) );
	EXPECT_EQ( has_0, ( IsTypeIndexValid_v<expected_size - 1, type_list_t> ) );
	constexpr bool const has_1 = 1 < expected_size;
	EXPECT_EQ( has_1, ( IsTypeIndexValid_v<1, type_list_t> ) );
	EXPECT_EQ( has_1, ( IsTypeIndexValid_v<expected_size - 2, type_list_t> ) );
	EXPECT_FALSE( ( IsTypeIndexValid_v<expected_size, type_list_t> ) );
	EXPECT_FALSE( ( IsTypeIndexValid_v<expected_size + 1, type_list_t> ) );
}

namespace
{
template<typename T, std::size_t TIndex, typename TList>
struct TypeListFindConfig
{
	using find_t      = T;
	using type_list_t = TList;

	static constexpr std::size_t const msc_index = TIndex;
};

// clang-format off
using Find_types = ::testing::Types<
	TypeListFindConfig<int, 0, TypeList<int>>,
	TypeListFindConfig<void, 1, TypeList<int>>,

	TypeListFindConfig<int, 0, TypeList<int, float>>,
	TypeListFindConfig<float, 1, TypeList<int, float>>,
	TypeListFindConfig<void, 2, TypeList<int, float>>,

	TypeListFindConfig<int, 0, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int &, 2, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int *, 4, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int &&, 6, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int const, 1, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int const &, 3, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int const *, 5, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<int const &&, 7, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,
	TypeListFindConfig<void, 8, TypeList<int, int const, int &, int const &, int *, int const *, int &&, int const &&>>,

	TypeListFindConfig<custom::Vector, 0, TypeList<custom::Vector, int>>,
	TypeListFindConfig<int, 1, TypeList<custom::Vector, int>>,
	TypeListFindConfig<void, 2, TypeList<custom::Vector, int>>,

	TypeListFindConfig<int, 0, TypeList<>>,
	TypeListFindConfig<void, 0, TypeList<>>
>;
// clang-format on
} // namespace

template<typename T>
struct CrtpTypeListFindT : public testing::Test
{
	static constexpr std::size_t const msc_index = T::msc_index;

	using find_t      = typename T::find_t;
	using type_list_t = typename T::type_list_t;
};

TYPED_TEST_SUITE( CrtpTypeListFindT, Find_types );

TYPED_TEST( CrtpTypeListFindT, find_type )
{
	using find_t      = typename TestFixture::find_t;
	using type_list_t = typename TestFixture::type_list_t;

	constexpr auto const expected = TestFixture::msc_index;

	auto const index = FindType_v<find_t, type_list_t>;
	EXPECT_EQ( expected, index );
}

namespace
{
template<typename T>
using is_str = std::is_same<std::string, T>;
template<typename T>
using is_cstr = std::is_same<std::string const, T>;
} // namespace

TEST( CrtpTypeList, find_type_if )
{
	using type_list_t = TypeList<int, int const, float const, std::string const>;

	{
		constexpr std::size_t const index = FindTypeIf_v<std::is_const, type_list_t>;
		using sot_t                       = FindTypeIf_t<std::is_const, type_list_t>;
		constexpr bool const valid        = std::is_same_v<int const, sot_t>;
		EXPECT_EQ( 1, index );
		EXPECT_TRUE( valid ) << "sot_t = " << typeid( sot_t ).name();
	}
	{
		constexpr std::size_t const index = FindTypeIf_v<is_cstr, type_list_t>;
		using sot_t                       = FindTypeIf_t<is_cstr, type_list_t>;
		constexpr bool const valid        = std::is_same_v<std::string const, sot_t>;
		EXPECT_EQ( 3, index );
		EXPECT_TRUE( valid ) << "sot_t = " << typeid( sot_t ).name();
	}
	{
		constexpr std::size_t const index = FindTypeIf_v<is_str, type_list_t>;
		using sot_t                       = FindTypeIf_t<is_str, type_list_t>;
		EXPECT_EQ( 4, index );
	}
}

TYPED_TEST( CrtpTypeListFindT, has_type )
{
	using find_t      = typename TestFixture::find_t;
	using type_list_t = typename TestFixture::type_list_t;

	constexpr auto const index = TestFixture::msc_index;
	constexpr auto const size  = TypeList_size<type_list_t>;

	constexpr bool contains = index < size;

	auto const has = HasType_v<find_t, type_list_t>;
	EXPECT_EQ( contains, has );
}

TYPED_TEST( CrtpTypeListFindT, type_at )
{
	using find_t      = typename TestFixture::find_t;
	using type_list_t = typename TestFixture::type_list_t;

	constexpr auto const index = TestFixture::msc_index;
	constexpr auto const size  = TypeList_size<type_list_t>;

	if constexpr ( IsTypeIndexValid_v<index, type_list_t> )
	{
		using sot_t = TypeAt_t<index, type_list_t>;
		bool same   = std::is_same_v<find_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}
}

TEST( CrtpTypeList, concat )
{
	using lhs_t      = TypeList<int, float>;
	using rhs_t      = TypeList<long, std::string>;
	using expected_t = TypeList<int, float, long, std::string>;

	using sot_t = TypeListConcat_t<lhs_t, rhs_t>;

	auto const same = std::is_same_v<expected_t, sot_t>;
	EXPECT_TRUE( same ) << typeid( sot_t ).name();
}

TEST( CrtpTypeList, push_back )
{
	using list_t      = TypeList<int, float>;
	using push_t      = TypeList<long, std::string>;
	using expected_t = TypeList<int, float, push_t>;

	using sot_t = TypeListPushBack_t<push_t, list_t>;

	auto const same = std::is_same_v<expected_t, sot_t>;
	EXPECT_TRUE( same ) << typeid( sot_t ).name();
}

TEST( CrtpTypeList, push_front )
{
	using list_t     = TypeList<int, float>;
	using push_t     = TypeList<long, std::string>;
	using expected_t = TypeList<push_t, int, float>;

	using sot_t = TypeListPushFront_t<push_t, list_t>;

	auto const same = std::is_same_v<expected_t, sot_t>;
	EXPECT_TRUE( same ) << typeid( sot_t ).name();
}

TEST( CrtpTypeList, flatten )
{
	using lhs_t      = TypeList<int, float>;
	using rhs_t      = TypeList<long, lhs_t, std::string>;
	using expected_t = TypeList<void, int, float, double, long, int, float, std::string>;

	using sot_t = TypeListFlatten_t<TypeList<void, lhs_t, double, rhs_t>>;

	auto const same = std::is_same_v<expected_t, sot_t>;
	EXPECT_TRUE( same ) << typeid( sot_t ).name();
}

TEST( CrtpTypeList, remove_at )
{
	using list_t = TypeList<void, int, float, double, long, std::string>;

	{
		using sot_t      = TypeListRemoveAt_t<-1, list_t>;
		using expected_t = list_t;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAt_t<0, list_t>;
		using expected_t = TypeList<int, float, double, long, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAt_t<3, list_t>;
		using expected_t = TypeList<void, int, float, long, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAt_t<5, list_t>;
		using expected_t = TypeList<void, int, float, double, long>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAt_t<6, list_t>;
		using expected_t = list_t;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}
}

TEST( CrtpTypeList, remove )
{
	using nested_t = TypeList<float, double, long>;
	using list_t   = TypeList<void, int, nested_t, std::string>;

	{
		using sot_t      = TypeListRemove_t<void*, list_t>;
		using expected_t = list_t;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<void, list_t>;
		using expected_t = TypeList<int, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<int, list_t>;
		using expected_t = TypeList<void, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<nested_t, list_t>;
		using expected_t = TypeList<void, int, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<std::string, list_t>;
		using expected_t = TypeList<void, int, nested_t>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<float, list_t>;
		using expected_t = TypeList<void, int, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<double, list_t>;
		using expected_t = TypeList<void, int, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemove_t<long, list_t>;
		using expected_t = TypeList<void, int, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}
}

TEST( CrtpTypeList, remove_list )
{
	using nested_t = TypeList<float, double, long>;
	using list_t   = TypeList<void, int, nested_t, std::string>;

	{
		using sot_t      = TypeListRemoveList_t<TypeList<void*>, list_t>;
		using expected_t = list_t;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<void>, list_t>;
		using expected_t = TypeList<int, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<void, int>, list_t>;
		using expected_t = TypeList<nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<void, std::string>, list_t>;
		using expected_t = TypeList<int, nested_t>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<int, std::string>, list_t>;
		using expected_t = TypeList<void, nested_t>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<std::string>, list_t>;
		using expected_t = TypeList<void, int, nested_t>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<nested_t>, list_t>;
		using expected_t = TypeList<void, int, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveList_t<TypeList<int>, list_t>;
		using expected_t = TypeList<void, nested_t, std::string>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}
}

TEST( CrtpTypeList, remove_all )
{
	using nested_t = TypeList<float, double, long>;
	using list_t   = TypeList<int, void, int, nested_t, std::string, int, nested_t, int>;

	{
		using sot_t      = TypeListRemoveAll_t<void*, list_t>;
		using expected_t = list_t;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAll_t<int, list_t>;
		using expected_t = TypeList<void, nested_t, std::string, nested_t>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAll_t<nested_t, list_t>;
		using expected_t = TypeList<int, void, int, std::string, int, int>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}

	{
		using sot_t      = TypeListRemoveAll_t<std::string, list_t>;
		using expected_t = TypeList<int, void, int, nested_t, int, nested_t, int>;

		auto const same = std::is_same_v<expected_t, sot_t>;
		EXPECT_TRUE( same ) << typeid( sot_t ).name();
	}
}

TEST( CrtpTypeList, make_set )
{
	using nested_t = TypeList<float, double, long>;
	using list_t   = TypeList<int, void, int, nested_t, std::string, int, nested_t, int>;

	using sot_t      = MakeTypeSet_t<list_t>;
	using expected_t = TypeList<int, void, nested_t, std::string>;

	auto const same = std::is_same_v<expected_t, sot_t>;
	EXPECT_TRUE( same ) << typeid( sot_t ).name();
}

TEST( CrtpTypeList, is_set )
{
	using nested_t = TypeList<float, double, long>;
	using list_t   = TypeList<int, void, int, nested_t, std::string, int, nested_t, int>;

	constexpr auto const no_set = IsTypeSet_v<list_t>;
	EXPECT_FALSE( no_set ) << typeid( list_t ).name();

	using sot_t      = MakeTypeSet_t<list_t>;
	constexpr auto const is_set = IsTypeSet_v<sot_t>;
	EXPECT_TRUE( is_set ) << typeid( sot_t ).name();
}

} // namespace crtp
