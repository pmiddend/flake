cmake_minimum_required(
  VERSION
  2.6)

project(
  flake)

include(
	FcpptCMakeUtils)

include(
	AwlMainGenerator)

find_package(
	Boost 1.45.0 REQUIRED COMPONENTS
	filesystem
	thread
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

add_definitions(
	-DBOOST_THREAD_VERSION=3)
# boost end

if(MSVC)
	add_definitions(
		-DUNICODE=1
		-D_UNICODE=1)

	# this in base init list
	# assigment operator generation
	add_definitions(
		/wd4355
		/wd4626)
endif()

# fcppt begin
find_package(
	fcppt REQUIRED)
include_directories(
	${fcppt_INCLUDE_DIRS})
add_definitions(
	${fcppt_DEFINITIONS})
# fcppt end

# cg begin
find_package(
	Cg REQUIRED)
include_directories(
	${Cg_INCLUDE_DIR})
add_definitions(
	${Cg_DEFINITIONS})
# cg end

# OpenCL begin
find_package(
	OpenCL REQUIRED)
include_directories(
	${OpenCL_INCLUDE_DIR})
add_definitions(
	${OpenCL_DEFINITIONS})
# OpenCL end

# cg begin
find_package(
	OpenGL REQUIRED)
include_directories(
	${OPENGL_INCLUDE_DIR})
# cg end

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
	awl REQUIRED)
include_directories(
	${awl_INCLUDE_DIR})
add_definitions(
	${awl_DEFINITIONS})
# awl end

# sge begin
find_package(
	sge REQUIRED COMPONENTS
	opencl
	config
	image
	image2d
	charconv
	input
	renderer
	log
	media
	scenic
	core
	sprite
	imagecolor
	shader
	camera
	texture
	cg
	renderercg
	rucksack
	image3d
	modelobj
	font
	fontdraw
	parsejson
	systems
	window
	viewport
	# All of the following are optional (transitive static)
	plugin
	audio
)

include_directories(
  ${sge_INCLUDE_DIRS})
add_definitions(
  ${sge_DEFINITIONS})
# sge end

# get rid of a pretty useless warning
if(CMAKE_COMPILER_IS_GNUCXX OR FCPPT_UTILS_COMPILER_IS_CLANGPP)
	add_definitions(
		-Wno-old-style-cast)
endif()

option(
	ENABLE_SHARED
	"Build flakelib as a shared library"
	TRUE)

option(
	ENABLE_STATIC
	"Build flakelib as a static library"
	FALSE)

# choose a library to link the tests to
if(
	ENABLE_SHARED)
	set(
		FLAKELIB_DEFAULT_LINK_TARGET
		flakelib)
endif()

# static overrides shared
if(
	ENABLE_STATIC)
	set(
		FLAKELIB_DEFAULT_LINK_TARGET
		flakelib_static)

	set(
		FLAKELIB_CONFIG_LINK_MACROS
		"-D FLAKELIB_STATIC_LINK")
endif()

add_subdirectory(
	flakelib)

add_subdirectory(
	flake)

add_subdirectory(
	tests)
