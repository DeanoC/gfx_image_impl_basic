cmake_minimum_required(VERSION 3.12.4)
get_directory_property(hasParent PARENT_DIRECTORY)
if(NOT hasParent)
	option(unittests "unittests" OFF)
	get_filename_component(_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
	set_property(GLOBAL PROPERTY GLOBAL_FETCHDEPS_BASE ${_PARENT_DIR}/al2o3 )
	include(FetchContent)
	FetchContent_Declare( al2o3 GIT_REPOSITORY https://github.com/DeanoC/al2o3 GIT_TAG master )
	FetchContent_GetProperties(al2o3)
	if(NOT al2o3_POPULATED)
		FetchContent_Populate(al2o3)
		add_subdirectory(${al2o3_SOURCE_DIR} ${al2o3_BINARY_DIR})
	endif()
	INIT_AL2O3(${CMAKE_CURRENT_SOURCE_DIR})
endif ()

SET_MIN_VERSIONS()
set(BaseInterfaceName gfx_image)
set(ImplName basic)
set(LibName ${BaseInterfaceName}_impl_${ImplName})
project(${LibName})

set(Interface
		)
set(Src
		convert.cpp
		create.cpp
		hq_resample.hpp
		image.cpp
		utils.cpp
		)

set(Deps
		al2o3_platform
		al2o3_cmath
		gfx_image_interface
		tiny_imageformat
		al2o3_memory
		)
ADD_LINK_TIME_IMPL(${BaseInterfaceName} ${ImplName} "${Interface}" "${Src}" "${Deps}")
set( Tests
		runner.cpp
		test_image.cpp
		test_pixel.cpp
		)
set( TestDeps
		al2o3_catch2
		utils_simple_logmanager
		)
ADD_LIB_TESTS(${BaseInterfaceName}_impl_${ImplName} "${Interface}" "${Tests}" "${TestDeps}")
