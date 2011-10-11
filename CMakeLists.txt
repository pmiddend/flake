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
	src/flakelib/media_path.cpp
	src/flakelib/media_path_from_string.cpp
	src/flakelib/planar_buffer.cpp
	src/flakelib/profiler/object.cpp
	src/flakelib/profiler/scoped.cpp
	src/flakelib/simulation/base.cpp
	src/flakelib/simulation/create.cpp
	src/flakelib/simulation/stam.cpp
	src/flakelib/utf8_file_to_fcppt_string.cpp
	src/flakelib/visualization/arrow.cpp
	src/flakelib/visualization/base.cpp
	src/flakelib/visualization/create.cpp
	src/flakelib/visualization/monitor/child.cpp
	src/flakelib/visualization/monitor/parent.cpp
	src/flakelib/visualization/monitor/planar_arrows.cpp
	src/flakelib/visualization/monitor/texture.cpp
	src/flakelib/visualization/monitor/window_manager.cpp
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
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	benchmark
	DESTINATION
	${INSTALL_BINARY_DIR})
# benchmark end
