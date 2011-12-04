CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(flake)

INCLUDE(FcpptCMakeUtils)

FIND_PACKAGE(
	Boost 1.45.0 REQUIRED COMPONENTS
	system
	date_time)

INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

IF(
	MSVC AND ${Boost_VERSION} STREQUAL "104800")
	ADD_DEFINITIONS(
		-D BOOST_FUSION_DONT_USE_PREPROCESSED_FILES
	)
ENDIF()
# boost end

# fcppt begin
FIND_PACKAGE(Fcppt REQUIRED)
INCLUDE_DIRECTORIES(${Fcppt_INCLUDE_DIRS})
ADD_DEFINITIONS(${Fcppt_DEFINITIONS})
# fcppt end

# rucksack begin
FIND_PACKAGE(Rucksack REQUIRED)
INCLUDE_DIRECTORIES(${Rucksack_INCLUDE_DIR})
ADD_DEFINITIONS(${Rucksack_DEFINITIONS})
# rucksack end

# OpenCL begin
FIND_PACKAGE(OpenCL REQUIRED)
INCLUDE_DIRECTORIES(${OPENCL_INCLUDE_DIR})
ADD_DEFINITIONS(${OpenCL_DEFINITIONS})
# OpenCL end

# sge begin
FIND_PACKAGE(
	SGE REQUIRED COMPONENTS
	opencl
	config
	image
	image2d
	input
	renderer
	sprite
	camera
	texture
	font
	fonttext
	parse
	shader
	systems
	window
	viewport)

INCLUDE_DIRECTORIES(${SGE_INCLUDE_DIRS})
ADD_DEFINITIONS(${SGE_DEFINITIONS})
# sge end

# get rid of a pretty useless warning
IF(CMAKE_COMPILER_IS_GNUCXX OR FCPPT_UTILS_COMPILER_IS_CLANGPP)
	ADD_DEFINITIONS(-Wno-old-style-cast)
ENDIF()

SET(
	INSTALL_MEDIA_DIR
	${INSTALL_DATA_DIR}/media)

INCLUDE(
	SGECustomPath)

SGE_CONFIG_ADD_CUSTOM_PATH(
	flake
	media
	${CMAKE_SOURCE_DIR}/media
	${INSTALL_MEDIA_DIR}
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

SET(
	FLAKELIB_FILES
	src/flakelib/cl/apply_kernel_to_planar_image.cpp
	src/flakelib/cl/planar_image_view_to_cl_image.cpp
	src/flakelib/density/advector.cpp
	src/flakelib/density/cursor_splatter.cpp
	src/flakelib/density/monitor_proxy.cpp
	src/flakelib/exception.cpp
	src/flakelib/laplace_solver/base.cpp
	src/flakelib/laplace_solver/dynamic_factory.cpp
	src/flakelib/laplace_solver/jacobi.cpp
	src/flakelib/laplace_solver/multigrid.cpp
	src/flakelib/laplace_tester.cpp
	src/flakelib/media_path.cpp
	src/flakelib/media_path_from_string.cpp
	src/flakelib/planar_buffer.cpp
	src/flakelib/planar_object_size.cpp
	src/flakelib/planar_pool/object.cpp
	src/flakelib/planar_pool/scoped_lock.cpp
	src/flakelib/profiler/object.cpp
	src/flakelib/profiler/scoped.cpp
	src/flakelib/scoped_frame_limiter.cpp
	src/flakelib/simulation/base.cpp
	src/flakelib/simulation/stam/object.cpp
	src/flakelib/sprite_drawer_3d.cpp
	src/flakelib/utf8_file_to_fcppt_string.cpp
	src/flakelib/utility/object.cpp
	src/flakelib/viewport_pager.cpp
	src/flakelib/visualization/arrow.cpp
	src/flakelib/visualization/base.cpp
	src/flakelib/visualization/monitor/child.cpp
	src/flakelib/visualization/monitor/parent.cpp
	src/flakelib/visualization/monitor/planar_arrows.cpp
	src/flakelib/visualization/monitor/texture.cpp
)

FCPPT_UTILS_ADD_SOURCE_GROUPS("${FLAKELIB_FILES}")

ADD_LIBRARY(
	flakelib STATIC ${FLAKELIB_FILES})

# main begin
ADD_EXECUTABLE(
	main
	src/flake/main.cpp)

TARGET_LINK_LIBRARIES(
	main
	flakelib
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	main
	DESTINATION
	${INSTALL_BINARY_DIR})
# main end

# benchmark begin
ADD_EXECUTABLE(
	benchmark
	src/flake/benchmark.cpp)

TARGET_LINK_LIBRARIES(
	benchmark
	flakelib
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	benchmark
	DESTINATION
	${INSTALL_BINARY_DIR})
# benchmark end

# pde_tester begin
ADD_EXECUTABLE(
	pde_tester
	src/flake/pde_tester.cpp)

TARGET_LINK_LIBRARIES(
	pde_tester
	flakelib
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	pde_tester
	DESTINATION
	${INSTALL_BINARY_DIR})
# pde_tester end

# frobenius_tester begin
ADD_EXECUTABLE(
	frobenius_tester
	src/flake/frobenius_tester.cpp)

TARGET_LINK_LIBRARIES(
	frobenius_tester
	flakelib
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	frobenius_tester
	DESTINATION
	${INSTALL_BINARY_DIR})
# frobenius_tester end
