cmake_minimum_required(VERSION 3.12)

project(crtp_mock
	LANGUAGES CXX
)

set(${PROJECT_NAME}_3rdparty_DIR "../../.."
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
include("${${PROJECT_NAME}_build_chain_DIR}/import_library.cmake")
include("${${PROJECT_NAME}_build_chain_DIR}/dependencies.cmake")

begin_project()

generate_target_id(target_id)
DefaultInstallPrefix(${target_id})

# external dependencies
Default_gtest()

set(crtp_3rdparty_DIR "${${PROJECT_NAME}_3rdparty_ABSDIR}" CACHE PATH "")
set(crtp_INCLUDE_SRC ON CACHE BOOL "")
GuardedImport(crtp
	"../src"
)

# project itself
DefineProjectInstallVariables()

set(src "${CMAKE_CURRENT_SOURCE_DIR}")
set(include "${CMAKE_CURRENT_SOURCE_DIR}")

set(sources
	"${src}/dummy.cpp"
)

set(headers_install
)

set(headers
	${headers_install}
)

add_library(${PROJECT_NAME}
	${sources}
	${headers}
)

group_sources("${sources};${headers}")

SetupDebugInformation(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 11
	CXX_STANDARD 20
)

set_target_properties(gtest PROPERTIES
	C_STANDARD 11
	CXX_STANDARD 20
)

set_target_properties(gtest_main PROPERTIES
	C_STANDARD 11
	CXX_STANDARD 20
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${include}>
		$<INSTALL_INTERFACE:${PROJECT_NAME}_INSTALL_INCLUDE>
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		crtp
		gmock
		gmock_main
)

DefaultProjectInstallAndExport()

end_project()
