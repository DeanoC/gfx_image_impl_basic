#if building standalone and not specified build tests
option(BUILD_TESTS "Whether to build tests" ON)
include(FetchContent)
FetchContent_Declare( al2o3 GIT_REPOSITORY https://github.com/DeanoC/al2o3 GIT_TAG master )
FetchContent_GetProperties(al2o3)
if(NOT al2o3_POPULATED)
	FetchContent_Populate(al2o3)
	add_subdirectory(${al2o3_SOURCE_DIR} ${al2o3_BINARY_DIR})
endif()

SET_MIN_VERSIONS()
set(BaseInterfaceName gfx_image)
set(ImplName basic)
set(LibName ${BaseInterfaceName}_impl_${ImplName})
project(${LibName})

set(Interface
		)
set(Src
		block.c
		convert.cpp
		create.cpp
		dds.hpp
		fetch.hpp
		hq_resample.hpp
		image.cpp
		put.hpp
		utils.cpp
		)

set(Deps
		gfx_image_interface
		gfx_imageformat
		al2o3_memory
		)
ADD_LINK_TIME_IMPL(${BaseInterfaceName} ${ImplName} "${Interface}" "${Src}" "${Deps}")
set( Tests
		runner.cpp
		test_image.cpp
		test_pixel.cpp
		)
set( TestDeps
		al2o3_catch2 )
ADD_LIB_TESTS(${BaseInterfaceName}_impl_${ImplName} "${Interface}" "${Tests}" "${TestDeps}")