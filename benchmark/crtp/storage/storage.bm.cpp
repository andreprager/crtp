
#include "custom.hpp"

#include <crtp/storage/builder/builder.hpp>
#include <crtp/storage/hybrid/hybrid.hpp>
#include <crtp/storage/on_heap/on_heap.hpp>
#include <crtp/storage/on_stack/on_stack.hpp>
#include <crtp/storage/storage.hpp>

#include <benchmark/benchmark.h>

#include <any>
#include <string>

constexpr auto const gsc_mul = 4;
constexpr auto const gsc_min = 1;
constexpr auto const gsc_max = 1 << 10;

using custom::Array;
using custom::call_user_api;
using custom::gs_array;
using custom::gs_vector;
using custom::UserApi;
using custom::UserApiConcept;
using custom::UserApiModel;
using custom::Vector;
using custom::ViewUserApi;

template<typename C>
struct Builder
{
	// static C build( std::size_t size, std::uint8_t v );
};

template<>
struct Builder<Vector>
{
	static Vector build( std::size_t size, std::uint8_t v )
	{
		return Vector{ size, v };
	}
};

template<std::size_t TSize, std::size_t TAlignment>
struct Builder<Array<TSize, TAlignment>>
{
	static Array<TSize, TAlignment> build( std::size_t size, std::uint8_t v )
	{
		return Array<TSize, TAlignment>{ v };
	}
};

template<typename T = void>
struct BuilderVector
{
	template<typename TStoragePolicy>
	static std::vector<UserApi<TStoragePolicy>> build( std::size_t size )
	{
		std::vector<UserApi<TStoragePolicy>> ret;
		for ( std::size_t i = 0; i < size; ++i )
		{
			if ( i % 2 )
			{
				ret.emplace_back( Vector{ 64, static_cast<std::uint8_t>( i ) } );
			}
			else
			{
				ret.emplace_back( Array<64>{ static_cast<std::uint8_t>( i ) } );
			}
		}
		return ret;
	}
};

template<>
struct BuilderVector<Vector>
{
	template<typename TStoragePolicy>
	static std::vector<UserApi<TStoragePolicy>> build( std::size_t size )
	{
		std::vector<UserApi<TStoragePolicy>> ret;
		for ( std::size_t i = 0; i < size; ++i )
		{
			ret.emplace_back( Vector{ 64, static_cast<std::uint8_t>( i ) } );
		}
		return ret;
	}
};

template<std::size_t TSize, std::size_t TAlignment>
struct BuilderVector<Array<TSize, TAlignment>>
{
	template<typename TStoragePolicy>
	static std::vector<UserApi<TStoragePolicy>> build( std::size_t size )
	{
		std::vector<UserApi<TStoragePolicy>> ret;
		for ( std::size_t i = 0; i < size; ++i )
		{
			ret.emplace_back( Array<TSize, TAlignment>{ static_cast<std::uint8_t>( i ) } );
		}
		return ret;
	}
};

namespace crtp::storage
{
using StorageUserApi = Builder<UserApiModel, UserApiConcept>;

static void BM_crtp_storage_setup( benchmark::State& state )
{
	for ( auto _ : state )
	{
		auto sot = ::Builder<Vector>::build( 256, static_cast<std::uint8_t>( 42 ) );
		benchmark::DoNotOptimize( sot );
	}
}

BENCHMARK( BM_crtp_storage_setup );

static void BM_crtp_storage_raw_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	for ( auto _ : state )
	{
		for ( std::size_t i = 0; i < size; ++i )
		{
			auto sot = ::Builder<Vector>::build( size, static_cast<std::uint8_t>( i ) );
			benchmark::DoNotOptimize( sot );
			call_user_api( sot );
			benchmark::DoNotOptimize( gs_vector );
		}
	}
}

BENCHMARK( BM_crtp_storage_raw_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

static void BM_crtp_storage_std_any_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	for ( auto _ : state )
	{
		for ( std::size_t i = 0; i < size; ++i )
		{
			std::any sot = ::Builder<Vector>::build( size, static_cast<std::uint8_t>( i ) );
			benchmark::DoNotOptimize( sot );
			call_user_api( *std::any_cast<Vector>( &sot ) );
			benchmark::DoNotOptimize( gs_vector );
		}
	}
}

BENCHMARK( BM_crtp_storage_std_any_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

static void BM_crtp_storage_OO_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	for ( auto _ : state )
	{
		for ( std::size_t i = 0; i < size; ++i )
		{
			std::unique_ptr<UserApiConcept> sot
			    = std::make_unique<UserApiModel<Vector>>( ::Builder<Vector>::build( size, static_cast<std::uint8_t>( i ) ) );
			benchmark::DoNotOptimize( sot );
			call_user_api( *sot );
			benchmark::DoNotOptimize( gs_vector );
		}
	}
}

BENCHMARK( BM_crtp_storage_OO_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

static void BM_crtp_storage_mvd_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	for ( auto _ : state )
	{
		for ( std::size_t i = 0; i < size; ++i )
		{
			auto sot = ::Builder<Vector>::build( size, static_cast<std::uint8_t>( i ) );
			benchmark::DoNotOptimize( sot );
			call_user_api( ViewUserApi{ sot } );
			benchmark::DoNotOptimize( gs_vector );
		}
	}
}

BENCHMARK( BM_crtp_storage_mvd_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

template<typename T, typename C>
static void BM_crtp_storage_user( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	for ( auto _ : state )
	{
		for ( std::size_t i = 0; i < size; ++i )
		{
			UserApi<T> sot = ::Builder<C>::build( size, static_cast<std::uint8_t>( i ) );
			benchmark::DoNotOptimize( sot );
			call_user_api( sot );
			benchmark::DoNotOptimize( gs_vector );
		}
	}
}

// clang-format off
BENCHMARK( BM_crtp_storage_user<OnHeap<StorageUserApi>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<OnStack<StorageUserApi, 64>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<OnStack<StorageUserApi, 128>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<OnStack<StorageUserApi, 256>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 8>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 16>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 32>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 64>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 128>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 256>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<OnHeap<StorageUserApi>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<OnStack<StorageUserApi, 128>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<OnStack<StorageUserApi, 256>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 8>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 16>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 32>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 64>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 128>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_user<Hybrid<StorageUserApi, 256>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
// clang-format on

template<typename T, typename C>
static void BM_crtp_storage_mvd( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	for ( auto _ : state )
	{
		for ( std::size_t i = 0; i < size; ++i )
		{
			UserApi<T> sot = ::Builder<C>::build( size, static_cast<std::uint8_t>( i ) );
			benchmark::DoNotOptimize( sot );
			ViewUserApi view{ sot };
			benchmark::DoNotOptimize( view );
			call_user_api( view );
			benchmark::DoNotOptimize( gs_vector );
		}
	}
}

// clang-format off
BENCHMARK( BM_crtp_storage_mvd<OnHeap<StorageUserApi>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<OnStack<StorageUserApi, 64>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<OnStack<StorageUserApi, 128>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<OnStack<StorageUserApi, 256>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 8>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 16>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 32>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 64>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 128>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 256>, Vector> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<OnHeap<StorageUserApi>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<OnStack<StorageUserApi, 128>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<OnStack<StorageUserApi, 256>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 8>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 16>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 32>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 64>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 128>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_mvd<Hybrid<StorageUserApi, 256>, Array<64>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
// clang-format on

/// std::vector

static void BM_crtp_storage_vector_raw_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	std::vector<Vector> sot;

	for ( std::size_t i = 0; i < size; ++i )
	{
		sot.emplace_back( Vector::sequence( size, static_cast<std::uint8_t>( i ) ) );
	}
	for ( auto _ : state )
	{
		for ( auto& entry : sot )
		{
			call_user_api( entry );
			benchmark::DoNotOptimize( gs_vector );
		}
		benchmark::DoNotOptimize( sot );
	}
}

BENCHMARK( BM_crtp_storage_vector_raw_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

static void BM_crtp_storage_vector_std_any_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	std::vector<std::any> sot;
	for ( std::size_t i = 0; i < size; ++i )
	{
		sot.emplace_back( Vector::sequence( size, static_cast<std::uint8_t>( i ) ) );
	}
	for ( auto _ : state )
	{
		for ( auto& entry : sot )
		{
			call_user_api( *std::any_cast<Vector>( &entry ) );
			benchmark::DoNotOptimize( gs_vector );
		}
		benchmark::DoNotOptimize( sot );
	}
}

BENCHMARK( BM_crtp_storage_vector_std_any_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

static void BM_crtp_storage_vector_OO_Vector( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	std::vector<std::unique_ptr<UserApiConcept>> sot;
	for ( std::size_t i = 0; i < size; ++i )
	{
		sot.emplace_back(
		    std::make_unique<UserApiModel<Vector>>( Vector::sequence( size, static_cast<std::uint8_t>( i ) ) ) );
	}
	for ( auto _ : state )
	{
		for ( auto& entry : sot )
		{
			call_user_api( *entry );
			benchmark::DoNotOptimize( gs_vector );
		}
		benchmark::DoNotOptimize( sot );
	}
}

BENCHMARK( BM_crtp_storage_vector_OO_Vector )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );

template<typename T, typename B>
static void BM_crtp_storage_vector_user( benchmark::State& state )
{
	std::size_t const size = state.range( 0 );

	auto sot = B::template build<T>( size );
	for ( auto _ : state )
	{
		for ( auto& entry : sot )
		{
			call_user_api( entry );
			benchmark::DoNotOptimize( gs_vector );
		}
		benchmark::DoNotOptimize( sot );
	}
}

// clang-format off
BENCHMARK( BM_crtp_storage_vector_user<OnHeap<StorageUserApi>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 64>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 128>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 256>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 8>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 16>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 32>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 64>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 128>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 256>, BuilderVector<Vector>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnHeap<StorageUserApi>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 128>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 256>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 8>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 16>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 32>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 64>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 128>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 256>, BuilderVector<Array<64>>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
// mixed
BENCHMARK( BM_crtp_storage_vector_user<OnHeap<StorageUserApi>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 128>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<OnStack<StorageUserApi, 256>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 8>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 16>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 32>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 64>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 128>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
BENCHMARK( BM_crtp_storage_vector_user<Hybrid<StorageUserApi, 256>, BuilderVector<>> )->RangeMultiplier( gsc_mul )->Range( gsc_min, gsc_max );
// clang-format on

} // namespace crtp::storage
