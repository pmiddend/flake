cmake_minimum_required(
  VERSION
  2.8.12)

project(
  flake)

include(
	FcpptCMakeUtils)

include(
	AwlMainGenerator)

find_package(
	Boost 1.45.0 REQUIRED COMPONENTS
	filesystem
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

find_package(
	fcppt REQUIRED)

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

# OpenGL begin
find_package(
	OpenGL REQUIRED)
include_directories(
	${OPENGL_INCLUDE_DIR})
# OpenGL end

find_package(
	awl REQUIRED)

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
	rucksackviewport
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
