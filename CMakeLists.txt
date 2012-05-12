cmake_minimum_required(VERSION 2.6)

project(flake)

include(
	FcpptCMakeUtils)

include(
	AwlMainGenerator)

include(
	FcpptSymbol
)

find_package(
	Boost 1.45.0 REQUIRED COMPONENTS
	chrono
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
	charconv
	input
	renderer
	log
	media
	core
	sprite
	camera
	texture
	cg
	renderercg
	image3d
	modelobj
	console
	font
	fonttext
	parse
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

include(
	SGECustomPath)

sge_config_add_custom_path(
	flake
	media
	${CMAKE_SOURCE_DIR}/media
	${INSTALL_DATA_DIR}/media
)

sge_config_add_custom_path(
	flakelib
	media
	${CMAKE_SOURCE_DIR}/flakelib_media
	${INSTALL_DATA_DIR}/flakelib_media
)

fcppt_generate_symbol_header(
	FLAKELIB_STATIC_LINK
	flakelib
	FLAKELIB
	flakelib
)

include_directories(
	${CMAKE_BINARY_DIR}/include)

include_directories(
	${CMAKE_SOURCE_DIR}/include)

set(
	FLAKELIB_FILES
	src/flakelib/buffer_pool/object.cpp
	src/flakelib/cl/cflags.cpp
	src/flakelib/cl/kernel.cpp
	src/flakelib/cl/planar_image_view_to_cl_image.cpp
	src/flakelib/cl/planar_image_view_to_float_buffer.cpp
	src/flakelib/cl/program.cpp
	src/flakelib/cl/program_context.cpp
	src/flakelib/console_percent_bar.cpp
	src/flakelib/exception.cpp
	src/flakelib/log.cpp
	src/flakelib/log_context.cpp
	src/flakelib/log_location.cpp
	src/flakelib/log_parameters.cpp
	src/flakelib/marching_cubes/manager.cpp
	src/flakelib/marching_cubes/object.cpp
	src/flakelib/marching_cubes/scan.cpp
	src/flakelib/media_path.cpp
	src/flakelib/media_path_from_string.cpp
	src/flakelib/planar/simulation/stam/buissnesq/object.cpp
	src/flakelib/planar/simulation/stam/divergence.cpp
	src/flakelib/planar/simulation/stam/jacobi.cpp
	src/flakelib/planar/simulation/stam/outflow_boundaries.cpp
	src/flakelib/planar/simulation/stam/semilagrangian_advection.cpp
	src/flakelib/planar/simulation/stam/subtract_pressure_gradient.cpp
	src/flakelib/planar/simulation/stam/vorticity.cpp
	src/flakelib/planar/simulation/stam/wind_source.cpp
	src/flakelib/scan/object.cpp
	src/flakelib/scoped_frame_limiter.cpp
	src/flakelib/splatter/box/object_fwd.cpp
	src/flakelib/splatter/object.cpp
	src/flakelib/splatter/rectangle/object.cpp
	src/flakelib/utf8_file_to_fcppt_string.cpp
	src/flakelib/utility/fill_buffer.cpp
	src/flakelib/utility/mix_buffers.cpp
	src/flakelib/value_modulator/object.cpp
	src/flakelib/volume/conversion/object.cpp
	src/flakelib/volume/gradient.cpp
	src/flakelib/volume/retrieve_filled_float_buffer.cpp
	src/flakelib/volume/retrieve_zero_float4_buffer.cpp
	src/flakelib/volume/simulation/stam/buissnesq/object.cpp
	src/flakelib/volume/simulation/stam/divergence.cpp
	src/flakelib/volume/simulation/stam/jacobi.cpp
	src/flakelib/volume/simulation/stam/outflow_boundaries.cpp
	src/flakelib/volume/simulation/stam/semilagrangian_advection.cpp
	src/flakelib/volume/simulation/stam/subtract_pressure_gradient.cpp
	src/flakelib/volume/simulation/stam/vorticity.cpp
	src/flakelib/volume/simulation/stam/wind_source.cpp
)

set(
	FLAKE_FILES
	src/flake/font_metrics_cache.cpp
	src/flake/media_path.cpp
	src/flake/media_path_from_string.cpp
	src/flake/notifications/message.cpp
	src/flake/notifications/object.cpp
	src/flake/planar/conversion/object.cpp
	src/flake/planar/cursor_splatter.cpp
	src/flake/planar/monitor/arrows.cpp
	src/flake/planar/monitor/child.cpp
	src/flake/planar/monitor/parent.cpp
	src/flake/planar/monitor/texture.cpp
	src/flake/save_l8_texture_to_file.cpp
	src/flake/sprite_drawer_3d.cpp
	src/flake/test/base.cpp
	src/flake/test/feature.cpp
	src/flake/test/information/manager.cpp
	src/flake/test/information/object.cpp
	src/flake/test/update_features_from_json.cpp
	src/flake/time_modifier/kernel.cpp
	src/flake/time_modifier/object.cpp
	src/flake/volume/arrows/manager.cpp
	src/flake/volume/arrows/object.cpp
	src/flake/volume/snow_cover/object.cpp
	src/flake/volume/snow_cover/scoped.cpp
	src/flake/shader/load_edited_string.cpp
	#src/flake/volume/density_visualization/raycaster/object.cpp
	src/flake/volume/flakes/manager.cpp
	src/flake/volume/flakes/mover.cpp
	src/flake/volume/model/manager.cpp
	src/flake/volume/model/object.cpp
	src/flake/volume/obstacles/manager.cpp
)

fcppt_utils_add_source_groups(
	"${FLAKELIB_FILES}")

fcppt_utils_add_source_groups(
	"${FLAKE_FILES}")

option(
	ENABLE_SHARED
	"Build flakelib as a shared library"
	TRUE
)

option(
	ENABLE_STATIC
	"Build flakelib as a static library"
	FALSE
)

if(
	NOT ENABLE_SHARED AND NOT ENABLE_STATIC
)
	message(
		FATAL_ERROR
		"You disabled both STATIC and SHARED"
	)
endif()

# choose a library to link the tests to
if(
	ENABLE_SHARED
)
	set(
		FLAKEKLIB_DEFAULT_LINK_TARGET
		flakelib
	)
endif()

# static overrides shared
if(
	ENABLE_STATIC
)
	set(
		FLAKELIB_DEFAULT_LINK_TARGET
		flakelib_static
	)

	set(
		FLAKELIB_CONFIG_LINK_MACROS
		"-D FLAKELIB_STATIC_LINK"
	)
endif()

function(
	MAKE_FLAKELIB_LIBRARY
	TARGETNAME
	VARIANT
)
	add_library(
		${TARGETNAME}
		${VARIANT}
		${FLAKELIB_FILES}
	)

	if(
		NOT MSVC
	)
		target_link_libraries(
			${TARGETNAME}
			${Boost_SYSTEM_LIBRARY}
		)
	endif()

	if(
		${VARIANT} STREQUAL "STATIC"
	)
		set_target_properties(
			${TARGETNAME}
			PROPERTIES
			COMPILE_DEFINITIONS
			"FLAKELIB_STATIC_LINK"
		)
	endif()

	if(UNIX)
		target_link_libraries(
			${TARGETNAME}
			ncurses)
	endif()


	# This is, of course, extremely overlinked
	target_link_libraries(
		${TARGETNAME}
		${Fcppt_core_LIBRARIES}
		${Fcppt_filesystem_LIBRARIES}
		${Awl_LIBRARIES}
		${SGE_LIBRARIES}
		${Rucksack_LIBRARIES}
		# FIXME FIXME FIXME
		OpenCL
		${Boost_LIBRARIES})

	install(
		TARGETS
		${TARGETNAME}
		DESTINATION
		${INSTALL_LIBRARY_DIR}
	)
endfunction()

if(
	ENABLE_STATIC
)
	make_flakelib_library(
		flakelib_static
		STATIC
	)
endif()

if(
	ENABLE_SHARED
)
	make_flakelib_library(
		flakelib
		SHARED
	)
endif()

add_library(
	flake_helper_library
	STATIC
	${FLAKE_FILES})

# vorticity begin
awl_utils_add_portable_executable(
	tests_vorticity
	"custom_main"
	src/flake/planar/tests/vorticity.cpp)

target_link_libraries(
	tests_vorticity
	flakelib
	flake_helper_library
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Awl_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_vorticity
	DESTINATION
	${INSTALL_BINARY_DIR})
# vorticity end

# buoyancy begin
awl_utils_add_portable_executable(
	tests_buoyancy
	"custom_main"
	src/flake/planar/tests/buoyancy.cpp)

target_link_libraries(
	tests_buoyancy
	flakelib
	flake_helper_library
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Awl_LIBRARIES}
	${SGE_LIBRARIES}
	${Rucksack_LIBRARIES}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_buoyancy
	DESTINATION
	${INSTALL_BINARY_DIR})
# buoyancy end
#
# flakes begin
#awl_utils_add_portable_executable(
#	tests_flakes
#	"custom_main"
#	src/flake/volume/tests/flakes.cpp)
#
#target_link_libraries(
#	tests_flakes
#	flakelib
#	flake_helper_library
#	${Fcppt_core_LIBRARIES}
#	${Fcppt_filesystem_LIBRARIES}
#	${Awl_LIBRARIES}
#	${SGE_LIBRARIES}
#	${Boost_LIBRARIES})
#
#install(
#	TARGETS
#	tests_flakes
#	DESTINATION
#	${INSTALL_BINARY_DIR})
# flakes end
#
## marching_cubes begin
awl_utils_add_portable_executable(
	tests_marching_cubes
	"custom_main"
	src/flake/volume/tests/marching_cubes.cpp)

target_link_libraries(
	tests_marching_cubes
	flakelib
	flake_helper_library
	${Fcppt_core_LIBRARIES}
	${Fcppt_filesystem_LIBRARIES}
	${Awl_LIBRARIES}
	${SGE_LIBRARIES}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_marching_cubes
	DESTINATION
	${INSTALL_BINARY_DIR})
## marching_cubes end
#
## density_visualization begin
#awl_utils_add_portable_executable(
#	tests_density_visualization
#	"custom_main"
#	src/flake/volume/tests/density_visualization.cpp)
#
#target_link_libraries(
#	tests_density_visualization
#	flakelib
#	flake_helper_library
#	${Fcppt_core_LIBRARIES}
#	${Fcppt_filesystem_LIBRARIES}
#	${Awl_LIBRARIES}
#	${SGE_LIBRARIES}
#	${Boost_LIBRARIES})
#
#install(
#	TARGETS
#	tests_density_visualization
#	DESTINATION
#	${INSTALL_BINARY_DIR})
## density_visualization end
#
## smoke begin
#awl_utils_add_portable_executable(
#	tests_smoke
#	"custom_main"
#	src/flake/volume/tests/smoke.cpp)
#
#target_link_libraries(
#	tests_smoke
#	flakelib
#	flake_helper_library
#	${Fcppt_core_LIBRARIES}
#	${Fcppt_filesystem_LIBRARIES}
#	${Awl_LIBRARIES}
#	${SGE_LIBRARIES}
#	${Boost_LIBRARIES})
#
#install(
#	TARGETS
#	tests_smoke
#	DESTINATION
#	${INSTALL_BINARY_DIR})
## smoke end
#
## solipsistic begin
#awl_utils_add_portable_executable(
#	tests_solipsistic
#	"custom_main"
#	src/flake/volume/tests/solipsistic.cpp)
#
#target_link_libraries(
#	tests_solipsistic
#	flakelib
#	flake_helper_library
#	${Fcppt_core_LIBRARIES}
#	${Fcppt_filesystem_LIBRARIES}
#	${Awl_LIBRARIES}
#	${SGE_LIBRARIES}
#	${Boost_LIBRARIES})
#
#install(
#	TARGETS
#	tests_solipsistic
#	DESTINATION
#	${INSTALL_BINARY_DIR})
## solipsistic end
#
## scan begin
#awl_utils_add_portable_executable(
#	tests_scan
#	"custom_main"
#	src/flake/volume/tests/scan.cpp)
#
#target_link_libraries(
#	tests_scan
#	flakelib
#	flake_helper_library
#	${Fcppt_core_LIBRARIES}
#	${Fcppt_filesystem_LIBRARIES}
#	${Awl_LIBRARIES}
#	${SGE_LIBRARIES}
#	${Boost_LIBRARIES})
#
#install(
#	TARGETS
#	tests_scan
#	DESTINATION
#	${INSTALL_BINARY_DIR})
# scan end
