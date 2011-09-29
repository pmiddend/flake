CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(flake)

INCLUDE(FcpptCMakeUtils)

FIND_PACKAGE(
	Boost 1.45.0 REQUIRED COMPONENTS
	system
	date_time)

INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
# boost end

# fcppt begin
FIND_PACKAGE(Fcppt REQUIRED)
INCLUDE_DIRECTORIES(${Fcppt_INCLUDE_DIRS})
ADD_DEFINITIONS(${Fcppt_DEFINITIONS})
# fcppt end

# awl begin
FIND_PACKAGE(Awl REQUIRED)
INCLUDE_DIRECTORIES(${Awl_INCLUDE_DIR})
ADD_DEFINITIONS(${Awl_DEFINITIONS})
# awl end

# mizuiro begin
FIND_PACKAGE(Mizuiro REQUIRED)
INCLUDE_DIRECTORIES(${Mizuiro_INCLUDE_DIR})
ADD_DEFINITIONS(${Mizuiro_DEFINITIONS})
# mizuiro end

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
	texture
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

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

SET(
	FLAKELIB_FILES
	src/flakelib/boundary_view.hpp
	src/flakelib/duration.hpp
	src/flakelib/media_path.cpp
	src/flakelib/media_path.hpp
	src/flakelib/media_path_from_string.cpp
	src/flakelib/media_path_from_string.hpp
	src/flakelib/profiler/call_count.hpp
	src/flakelib/profiler/child_sequence.hpp
	src/flakelib/profiler/clock.hpp
	src/flakelib/profiler/duration.hpp
	src/flakelib/profiler/object.cpp
	src/flakelib/profiler/object.hpp
	src/flakelib/profiler/object_fwd.hpp
	src/flakelib/profiler/optional_parent.hpp
	src/flakelib/profiler/scoped.cpp
	src/flakelib/profiler/scoped.hpp
	src/flakelib/profiler/scoped_fwd.hpp
	src/flakelib/profiler/time_point.hpp
	src/flakelib/simulation/base.cpp
	src/flakelib/simulation/base.hpp
	src/flakelib/simulation/base_fwd.hpp
	src/flakelib/simulation/base_ptr.hpp
	src/flakelib/simulation/create.cpp
	src/flakelib/simulation/create.hpp
	src/flakelib/simulation/stam.cpp
	src/flakelib/simulation/stam.hpp
	src/flakelib/utf8_file_to_fcppt_string.cpp
	src/flakelib/utf8_file_to_fcppt_string.hpp
	src/flakelib/visualization/arrow.cpp
	src/flakelib/visualization/arrow.hpp
	src/flakelib/visualization/arrow_vf/color.hpp
	src/flakelib/visualization/arrow_vf/format.hpp
	src/flakelib/visualization/arrow_vf/part.hpp
	src/flakelib/visualization/arrow_vf/position.hpp
	src/flakelib/visualization/base.cpp
	src/flakelib/visualization/base.hpp
	src/flakelib/visualization/base_fwd.hpp
	src/flakelib/visualization/base_ptr.hpp
	src/flakelib/visualization/create.cpp
	src/flakelib/visualization/create.hpp
	src/flakelib/visualization/dummy_sprite/choices.hpp
	src/flakelib/visualization/dummy_sprite/object.hpp
	src/flakelib/visualization/dummy_sprite/parameters.hpp
	src/flakelib/visualization/dummy_sprite/system.hpp
)

FCPPT_UTILS_ADD_SOURCE_GROUPS("${FLAKELIB_FILES}")

ADD_LIBRARY(
	flakelib STATIC ${FLAKELIB_FILES})

# app begin
ADD_EXECUTABLE(
	main
	src/flake/main.cpp)

TARGET_LINK_LIBRARIES(
	main
	flakelib
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	main
	DESTINATION
	${INSTALL_BINARY_DIR})
# app end
