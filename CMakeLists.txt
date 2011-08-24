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
	camera
	config
	fontbitmap
	fonttext
	image
	image2d
	input
	renderer
	line_drawer
	parse 
	shader
	sprite 
	systems
	texture
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

SET(
	MAIN_FILES
	src/main.cpp
	src/texture_from_scalar_grid.cpp
	src/simulation.cpp
	src/simulation_impl.cpp
	src/media_path.cpp)

FCPPT_UTILS_ADD_SOURCE_GROUPS("${MAIN_FILES}")

# app begin
ADD_EXECUTABLE(
	main
	${MAIN_FILES} src/main.cpp)

TARGET_LINK_LIBRARIES(
	main
	${Fcppt_LIBRARIES}
	${SGE_LIBRARIES}
	${Boost_LIBRARIES})

INSTALL(
	TARGETS
	main
	DESTINATION
	${INSTALL_BINARY_DIR})
# app end
