cmake_minimum_required(VERSION 3.12)

project(crtp
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
include("${${PROJECT_NAME}_build_chain_DIR}/import_library.cmake")
include("${${PROJECT_NAME}_build_chain_DIR}/dependencies.cmake")

begin_project()

generate_target_id(target_id)
DefaultInstallPrefix(${target_id})

# project itself
DefineProjectInstallVariables()

set(src "${CMAKE_CURRENT_SOURCE_DIR}")
set(include "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(natvis "${CMAKE_CURRENT_SOURCE_DIR}/../natvis")

set(sources
)

set(headers_private
)

set(headers_install
	"${include}/crtp/self.hpp"
	"${include}/crtp/storage/builder/builder.hpp"
	"${include}/crtp/storage/builder/builder.decl.hpp"
	"${include}/crtp/storage/builder/builder.impl.hpp"
	"${include}/crtp/storage/concept/concept.hpp"
	"${include}/crtp/storage/concept/concept.decl.hpp"
	"${include}/crtp/storage/concept/concept.impl.hpp"
	"${include}/crtp/storage/hybrid/hybrid.hpp"
	"${include}/crtp/storage/hybrid/hybrid.decl.hpp"
	"${include}/crtp/storage/hybrid/hybrid.impl.hpp"
	"${include}/crtp/storage/on_heap/on_heap.hpp"
	"${include}/crtp/storage/on_heap/on_heap.decl.hpp"
	"${include}/crtp/storage/on_heap/on_heap.impl.hpp"
	"${include}/crtp/storage/on_stack/on_stack.hpp"
	"${include}/crtp/storage/on_stack/on_stack.decl.hpp"
	"${include}/crtp/storage/on_stack/on_stack.impl.hpp"
	"${include}/crtp/storage/storage.hpp"
	"${include}/crtp/storage/storage.decl.hpp"
	"${include}/crtp/storage/storage.impl.hpp"
	"${include}/crtp/storage/swap/swap.hpp"
	"${include}/crtp/storage/swap/swap.decl.hpp"
	"${include}/crtp/storage/swap/swap.impl.hpp"
	"${include}/crtp/storage/traits/traits.hpp"
	"${include}/crtp/storage/value/concept/concept.hpp"
	"${include}/crtp/storage/value/concept/concept.decl.hpp"
	"${include}/crtp/storage/value/concept/concept.impl.hpp"
	"${include}/crtp/storage/value/value.hpp"
	"${include}/crtp/storage/value/value.decl.hpp"
	"${include}/crtp/storage/value/value.impl.hpp"
)

set(resources_natvis
	"${natvis}/crtp.natvis"
)

add_library(${PROJECT_NAME} INTERFACE
	${sources}
	${headers_install}
	${headers_private}
	${resources_natvis}
)

group_sources("${sources}" "${src}")
group_sources("${headers_private}" "${src}")
group_sources("${headers_install}" "${include}")
group_sources("${resources_natvis}" "${src}/..")

target_compile_options(${PROJECT_NAME}
	INTERFACE
		$<$<CXX_COMPILER_ID:MSVC>:/MP>
		$<$<CXX_COMPILER_ID:MSVC>:/permissive->
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 11
	CXX_STANDARD 20
)

set(${PROJECT_NAME}_INCLUDE_SRC OFF
	CACHE BOOL "Add directory ${src} to include BUILD_INTERFACE of ${PROJECT_NAME}.")

target_include_directories(${PROJECT_NAME}
	INTERFACE
		$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${${PROJECT_NAME}_INSTALL_PREFIX}${${PROJECT_NAME}_INSTALL_INCLUDE}>
		$<BUILD_INTERFACE:${include}>
		$<$<BOOL:${${PROJECT_NAME}_INCLUDE_SRC}>:$<BUILD_INTERFACE:${src}>>
		"${src}"
)

target_compile_definitions(${PROJECT_NAME}
	INTERFACE
		BUILD_VERSION="${BUILD_VERSION}"
		BUILD_NUMBER="${BUILD_NUMBER}"
#		_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
)

if(BUILD_SHARED_LIBS)
	target_compile_definitions(${PROJECT_NAME}
		INTERFACE
			${PROJECT_NAME}_SHARED
	)
else()
	target_compile_definitions(${PROJECT_NAME}
		INTERFACE
			${PROJECT_NAME}_STATIC
	)
endif()

DefaultProjectInstallAndExport()

#############################################
## Add Doxygen for crtp
set(${PROJECT_NAME}_BUILD_DOC OFF CACHE BOOL "Build documentation of ${PROJECT_NAME}.")
if(${PROJECT_NAME}_BUILD_DOC)
	set(${PROJECT_NAME}_DOC_INSTALL_PREFIX ${${PROJECT_NAME}_INSTALL_PREFIX}docs/ CACHE STRING
		"Install directory for documentation of ${PROJECT_NAME}.")
	mark_as_advanced(${PROJECT_NAME}_DOC_INSTALL_PREFIX)
	message(STATUS "  >> Doxygen ${PROJECT_NAME} >>")
	message(STATUS "Doxygen install path: ${${PROJECT_NAME}_DOC_INSTALL_PREFIX}")
	find_package(Doxygen)
	if(DOXYGEN_FOUND)
		message(STATUS "Generate documentation for ${PROJECT_NAME}...")
		add_custom_target( ${PROJECT_NAME}_doxygen ALL
			COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxyfile.txt"
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doxygen"
			COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen"
			VERBATIM
		)
		install(DIRECTORY
				"${CMAKE_CURRENT_SOURCE_DIR}/doxygen/${PROJECT_NAME}/html/"
			DESTINATION
				"${${PROJECT_NAME}_DOC_INSTALL_PREFIX}html"
			COMPONENT
				${PROJECT_NAME}
		)
	elseif()
		message(WARNING "Doxygen not found. Documentation cannot be generated.  >>")
	endif()
	message(STATUS "  << Doxygen <<")
endif()

end_project()
