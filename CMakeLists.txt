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

# awl begin
FIND_PACKAGE(Awl REQUIRED)
INCLUDE_DIRECTORIES(${Awl_INCLUDE_DIR})
ADD_DEFINITIONS(${Awl_DEFINITIONS})
# awl end

# sge begin
FIND_PACKAGE(
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
	src/flakelib/buffer_pool/object.cpp
	src/flakelib/cl/apply_kernel_to_planar_image.cpp
	src/flakelib/cl/planar_image_view_to_cl_image.cpp
	src/flakelib/cl/planar_image_view_to_float_buffer.cpp
	src/flakelib/exception.cpp
	src/flakelib/media_path.cpp
	src/flakelib/media_path_from_string.cpp
	src/flakelib/planar/buoyancy/object.cpp
	src/flakelib/planar/density/advector.cpp
	src/flakelib/planar/density/cursor_splatter.cpp
	src/flakelib/planar/density/monitor_proxy.cpp
	src/flakelib/planar/framework.cpp
	src/flakelib/planar/laplace_solver/base.cpp
	src/flakelib/planar/laplace_solver/dynamic_factory.cpp
	src/flakelib/planar/laplace_solver/jacobi.cpp
	src/flakelib/planar/laplace_solver/multigrid.cpp
	src/flakelib/planar/laplace_tester.cpp
	src/flakelib/planar/monitor/child.cpp
	src/flakelib/planar/monitor/parent.cpp
	src/flakelib/planar/monitor/planar_arrows.cpp
	src/flakelib/planar/monitor/planar_converter.cpp
	src/flakelib/planar/monitor/texture.cpp
	src/flakelib/planar/simulation/base.cpp
	src/flakelib/planar/simulation/stam/gravity_source.cpp
	src/flakelib/planar/simulation/stam/object.cpp
	src/flakelib/planar/simulation/stam/wind_source.cpp
	src/flakelib/profiler/object.cpp
	src/flakelib/profiler/scoped.cpp
	src/flakelib/scoped_frame_limiter.cpp
	src/flakelib/sprite_drawer_3d.cpp
	src/flakelib/utf8_file_to_fcppt_string.cpp
	src/flakelib/utility/object.cpp
	src/flakelib/viewport_pager.cpp
	src/flakelib/volume/boundary/cube/object.cpp
	src/flakelib/volume/boundary/json/to_cube.cpp
	src/flakelib/volume/boundary/json/to_obstacle_sequence.cpp
	src/flakelib/volume/boundary/json/to_obstacle_variant.cpp
	src/flakelib/volume/boundary/json/to_sphere.cpp
	src/flakelib/volume/boundary/object.cpp
	src/flakelib/volume/boundary/sphere/object.cpp
	src/flakelib/volume/conversion/object.cpp
	src/flakelib/volume/density/advector.cpp
	src/flakelib/volume/density/strength_modulator.cpp
	src/flakelib/volume/density/visual.cpp
	src/flakelib/volume/flake_simulation.cpp
	src/flakelib/volume/flakes/object.cpp
	src/flakelib/volume/laplace_solver/base.cpp
	src/flakelib/volume/laplace_solver/jacobi.cpp
	src/flakelib/volume/simulation/stam/object.cpp
	src/flakelib/volume/simulation/stam/wind_blower.cpp
	src/flakelib/volume/smoke_simulation.cpp
	src/flakelib/volume/visualization/arrows.cpp
	src/flakelib/volume/visualization/arrows_manager.cpp
	src/flakelib/volume/visualization/compiled_model.cpp
	src/flakelib/volume/visualization/ground.cpp
	src/flakelib/volume/visualization/shape_manager.cpp
)

FCPPT_UTILS_ADD_SOURCE_GROUPS("${FLAKELIB_FILES}")

ADD_LIBRARY(
	flakelib STATIC ${FLAKELIB_FILES})

# planar_framework begin
ADD_EXECUTABLE(
	planar_framework
	WIN32
	src/flake/planar_framework.cpp)

TARGET_LINK_LIBRARIES(
	planar_framework
	flakelib
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	planar_framework
	DESTINATION
	${INSTALL_BINARY_DIR})
# planar_framework end

# pde_tester begin
ADD_EXECUTABLE(
	pde_tester
	WIN32
	src/flake/pde_tester.cpp)

TARGET_LINK_LIBRARIES(
	pde_tester
	flakelib
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	pde_tester
	DESTINATION
	${INSTALL_BINARY_DIR})
# pde_tester end

# volume_framework begin
ADD_EXECUTABLE(
	volume_flake_simulation
	WIN32
	src/flake/volume_flake_simulation.cpp)

TARGET_LINK_LIBRARIES(
	volume_flake_simulation
	flakelib
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${SGE_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	volume_flake_simulation
	DESTINATION
	${INSTALL_BINARY_DIR})
# volume_test end

# volume_framework begin
ADD_EXECUTABLE(
	volume_smoke_simulation
	WIN32
	src/flake/volume_smoke_simulation.cpp)

TARGET_LINK_LIBRARIES(
	volume_smoke_simulation
	flakelib
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${SGE_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	volume_smoke_simulation
	DESTINATION
	${INSTALL_BINARY_DIR})
# volume_test end
