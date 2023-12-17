cmake_minimum_required(VERSION 3.12)

project(crtp_master)

set(${PROJECT_NAME}_3rdparty_DIR ".."
	CACHE PATH "Path to 3rd party directory for ${PROJECT_NAME} files.")

get_filename_component(${PROJECT_NAME}_3rdparty_ABSDIR "${${PROJECT_NAME}_3rdparty_DIR}" ABSOLUTE)

if(NOT EXISTS "${${PROJECT_NAME}_3rdparty_ABSDIR}")
	message(FATAL_ERROR "FAILURE: 3rd_party directory: ${${PROJECT_NAME}_3rdparty_ABSDIR}")
endif()
set(${PROJECT_NAME}_build_chain_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}/build_chain/cmake"
	CACHE PATH "Path to cmake helper files.")
if(NOT EXISTS "${${PROJECT_NAME}_build_chain_DIR}")
	message(FATAL_ERROR "FAILURE: build chain directory: ${${PROJECT_NAME}_build_chain_DIR}")
endif()

mark_as_advanced(
	${PROJECT_NAME}_3rdparty_DIR
	${PROJECT_NAME}_build_chain_DIR
)

include("${${PROJECT_NAME}_build_chain_DIR}/system.cmake")
include("${${PROJECT_NAME}_build_chain_DIR}/import_library.cmake")
include("${${PROJECT_NAME}_build_chain_DIR}/dependencies.cmake")

begin_project()

set(crtp_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
GuardedImport(crtp
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_BINARY_DIR}/crtp"
)

set(${PROJECT_NAME}_BUILD_TEST ON CACHE BOOL "Build test project for ${PROJECT_NAME}")
if(${PROJECT_NAME}_BUILD_TEST)
	set(gtest_force_shared_crt ON CACHE BOOL "")
	set(crtp_mock_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
	GuardedImport(crtp_mock
		"${CMAKE_CURRENT_SOURCE_DIR}/mock"
		"${CMAKE_CURRENT_BINARY_DIR}/crtp_mock"
	)

	set(crtp_unit_test_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
	GuardedImport(crtp_unit_test
		"${CMAKE_CURRENT_SOURCE_DIR}/unit_test"
		"${CMAKE_CURRENT_BINARY_DIR}/crtp_unit_test"
	)

	#############################################
	# Set default project to crtp_unit_test
	set_property(
		DIRECTORY
			"${CMAKE_CURRENT_SOURCE_DIR}/unit_test"
		PROPERTY
			VS_STARTUP_PROJECT crtp_unit_test
	)
endif()

set(${PROJECT_NAME}_BUILD_BENCHMARK ON CACHE BOOL "Build benchmark project for ${PROJECT_NAME}")
if(${PROJECT_NAME}_BUILD_BENCHMARK)
	set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "")
	set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "")
	set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "")
	set(crtp_benchmark_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
	GuardedImport(crtp_benchmark
		"${CMAKE_CURRENT_SOURCE_DIR}/benchmark"
		"${CMAKE_CURRENT_BINARY_DIR}/crtp_benchmark"
	)
endif()

end_project()
