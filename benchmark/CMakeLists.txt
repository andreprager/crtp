cmake_minimum_required(VERSION 3.10)

project(crtp_benchmark
	LANGUAGES CXX
)

set(${PROJECT_NAME}_3rdparty_DIR "../.."
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
include("${${PROJECT_NAME}_build_chain_DIR}/default_library.cmake")
include("${${PROJECT_NAME}_build_chain_DIR}/dependencies.cmake")

begin_project()

get_architecture(arch)
generate_target_id(target_id)
get_cxx_compiler(compiler)

DefaultInstallPrefix(${target_id})

# external dependencies
Default_gbenchmark()
	
set(crtp_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
GuardedImportConfigurable(${PROJECT_NAME} crtp
	"${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

set(crtp_mock_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
GuardedImport(crtp_mock
	"../mock"
)

# project itself

DefineProjectInstallVariables()
set(src "${CMAKE_CURRENT_SOURCE_DIR}")
set(include "${CMAKE_CURRENT_SOURCE_DIR}")
set(src_unit_test "${CMAKE_CURRENT_SOURCE_DIR}/../unit_test")

set(sources
	${src}/main.cpp
	${src}/crtp/storage/storage.bm.cpp
)

set(headers_install
)

set(headers_private
)

set_source_files_properties(${sources} PROPERTIES
	LANGUAGE CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<CONFIG>/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY $<CONFIG>/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY $<CONFIG>/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY $<CONFIG>/bin)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY $<CONFIG>/lib)

add_executable(${PROJECT_NAME}
	${sources}
	${headers_private}
	${headers_install}
	${sources_unit_test}
	${headers_unit_test}
)

group_sources("${sources}" ${src})
group_sources("${headers_private}" ${src})
group_sources("${headers_install}" ${include})
group_sources("${sources_unit_test}" ${src_unit_test})
group_sources("${headers_unit_test}" ${src_unit_test})

SetupDebugInformation(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if(compiler MATCHES "^(MSVC)")
	if(arch STREQUAL x86)
		target_link_options(${PROJECT_NAME}
			PUBLIC
				/LARGEADDRESSAWARE
		)
	endif()
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_INCLUDE}>
		$<BUILD_INTERFACE:${include}>
	PRIVATE
		${src}
		${src_unit_test}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY
	PUBLIC_HEADER
		${headers_install}
)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC
		BUILD_VERSION="${BUILD_VERSION}"
		BUILD_NUMBER="${BUILD_NUMBER}"
)

if(BUILD_SHARED_LIBS)
	target_compile_definitions(${PROJECT_NAME}
		PRIVATE
			${PROJECT_NAME}_SHARED
	)
else()
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC
			${PROJECT_NAME}_STATIC
	)
endif()

target_link_libraries(${PROJECT_NAME}
#	PUBLIC
	PRIVATE
		crtp
		crtp_mock
		benchmark
		benchmark_main
)

DefaultProjectInstallAndExport()

foreach(target
	crtp
	crtp_mock
	benchmark
	benchmark_main
)
	AddRuntimeDependencies(${PROJECT_NAME} ${target})
	if(${PROJECT_NAME}_INSTALL_DEPENDENCIES)
		InstallRuntimeDependencies(${PROJECT_NAME} ${target} ${${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_RUNTIME})
	endif()
endforeach()

message(STATUS "  << ${PROJECT_NAME} <<")
