cmake_minimum_required(VERSION 2.6)

project(flake)

include(
	FcpptCMakeUtils)
include(
	AwlMainGenerator)

find_package(
	Boost 1.45.0 REQUIRED COMPONENTS
	system)

include_directories(
	${Boost_INCLUDE_DIR})
link_directories(
	${Boost_LIBRARY_DIRS})

# boost-1.48 is broken with fusion, workaround
if(
	MSVC AND ${Boost_VERSION} STREQUAL "104800")
	add_definitions(
		-D BOOST_FUSION_DONT_USE_PREPROCESSED_FILES
	)
endif()
# boost end

# fcppt begin
find_package(
	Fcppt REQUIRED)
include_directories(
	${Fcppt_INCLUDE_DIRS})
add_definitions(
	${Fcppt_DEFINITIONS})
# fcppt end

# rucksack begin
find_package(
	Rucksack REQUIRED)
include_directories(
	${Rucksack_INCLUDE_DIR})
add_definitions(
	${Rucksack_DEFINITIONS})
# rucksack end

# OpenCL begin
find_package(
	OpenCL REQUIRED)
include_directories(
	${FCPPT_UTILS_INCLUDE_SYSTEM}
	${OPENCL_INCLUDE_DIR})
add_definitions(
	${OpenCL_DEFINITIONS})
# OpenCL end

# awl begin
find_package(
	Awl REQUIRED)
include_directories(
	${Awl_INCLUDE_DIR})
add_definitions(
	${Awl_DEFINITIONS})
# awl end

# sge begin
find_package(
	SGE REQUIRED COMPONENTS
	opencl
	config
	image
	image2d
	input
	renderer
	log
	media
	core
	sprite
	camera
	texture
	modelobj
	console
	font
	fonttext
	parse
	shader
	systems
	window
	viewport)

include_directories(${SGE_INCLUDE_DIRS})
add_definitions(${SGE_DEFINITIONS})
# sge end

# get rid of a pretty useless warning
if(CMAKE_COMPILER_IS_GNUCXX OR FCPPT_UTILS_COMPILER_IS_CLANGPP)
	add_definitions(
		-Wno-old-style-cast)
endif()

set(
	INSTALL_MEDIA_DIR
	${INSTALL_DATA_DIR}/media)

include(
	SGECustomPath)

sge_config_add_custom_path(
	flake
	media
	${CMAKE_SOURCE_DIR}/media
	${INSTALL_MEDIA_DIR}
)

include_directories(
	${CMAKE_SOURCE_DIR}/include)

set(
	FLAKELIB_FILES
	src/flakelib/buffer_pool/object.cpp
	src/flakelib/cl/kernel.cpp
	src/flakelib/cl/planar_image_view_to_cl_image.cpp
	src/flakelib/cl/planar_image_view_to_float_buffer.cpp
	src/flakelib/cl/program.cpp
	src/flakelib/cl/program_context.cpp
	src/flakelib/exception.cpp
	src/flakelib/log.cpp
	src/flakelib/log_context.cpp
	src/flakelib/log_location.cpp
	src/flakelib/log_parameters.cpp
	src/flakelib/media_path.cpp
	src/flakelib/media_path_from_string.cpp
	src/flakelib/planar/simulation/stam/divergence.cpp
	src/flakelib/planar/simulation/stam/jacobi.cpp
	src/flakelib/planar/simulation/stam/outflow_boundaries.cpp
	src/flakelib/planar/simulation/stam/semilagrangian_advection.cpp
	src/flakelib/planar/simulation/stam/subtract_pressure_gradient.cpp
	src/flakelib/planar/simulation/stam/wind_source.cpp
	src/flakelib/scoped_frame_limiter.cpp
	src/flakelib/utf8_file_to_fcppt_string.cpp
	src/flakelib/utility/fill_buffer.cpp
)

set(
	FLAKE_FILES
	src/flake/planar/monitor/child.cpp
	src/flake/planar/monitor/parent.cpp
	src/flake/planar/monitor/planar_arrows.cpp
	src/flake/planar/monitor/planar_converter.cpp
	src/flake/planar/monitor/texture.cpp
	src/flake/planar/tests/simple.cpp
	src/flake/sprite_drawer_3d.cpp
	src/flake/test_base.cpp
)

fcppt_utils_add_source_groups(
	"${FLAKELIB_FILES}")

add_library(
	flakelib STATIC ${FLAKELIB_FILES})

# simple begin
awl_utils_add_portable_executable(
	tests_simple
	"custom_main"
	src/flake/planar/tests/simple.cpp
	${FLAKE_FILES})

target_link_libraries(
	tests_simple
	flakelib
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Awl_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_simple
	DESTINATION
	${INSTALL_BINARY_DIR})
# simple end
