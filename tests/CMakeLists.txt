include_directories(
	${CMAKE_SOURCE_DIR}/flakelib/include)

include_directories(
	${CMAKE_SOURCE_DIR}/flake/include)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(
	${CMAKE_BINARY_DIR}/include)

# vorticity begin
awl_utils_add_portable_executable(
	tests_vorticity
	"custom_main"
	src/tests/vorticity.cpp)

target_link_libraries(
	tests_vorticity
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${Cg_LIBRARIES}
	${awl_TARGET}
	${sge_TARGETS}
	${OPENCL_LIBRARIES}
	${OPENGL_gl_LIBRARY}
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
	src/tests/buoyancy.cpp)

target_link_libraries(
	tests_buoyancy
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${awl_TARGET}
	${Cg_LIBRARIES}
	${sge_TARGETS}
	${OPENCL_LIBRARIES}
	${OPENGL_gl_LIBRARY}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_buoyancy
	DESTINATION
	${INSTALL_BINARY_DIR})
# buoyancy end
#
# flakes begin
awl_utils_add_portable_executable(
	tests_flakes
	"custom_main"
	src/tests/flakes.cpp)

target_link_libraries(
	tests_flakes
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${Cg_LIBRARIES}
	${OPENCL_LIBRARIES}
	${awl_TARGET}
	${OPENGL_gl_LIBRARY}
	${sge_TARGETS}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_flakes
	DESTINATION
	${INSTALL_BINARY_DIR})
# flakes end
#
## marching_cubes begin
awl_utils_add_portable_executable(
	tests_marching_cubes
	"custom_main"
	src/tests/marching_cubes.cpp)

target_link_libraries(
	tests_marching_cubes
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${Cg_LIBRARIES}
	${awl_TARGET}
	${sge_TARGETS}
	${OPENGL_gl_LIBRARY}
	${OPENCL_LIBRARIES}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_marching_cubes
	DESTINATION
	${INSTALL_BINARY_DIR})
## marching_cubes end
#
## density_visualization begin
awl_utils_add_portable_executable(
	tests_density_visualization
	"custom_main"
	src/tests/density_visualization.cpp)

target_link_libraries(
	tests_density_visualization
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${Cg_LIBRARIES}
	${awl_TARGET}
	${sge_TARGETS}
	${OPENCL_LIBRARIES}
	${OPENGL_gl_LIBRARY}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_density_visualization
	DESTINATION
	${INSTALL_BINARY_DIR})
## density_visualization end
#
#
## solipsistic begin
awl_utils_add_portable_executable(
	tests_solipsistic
	"custom_main"
	src/tests/solipsistic.cpp)

target_link_libraries(
	tests_solipsistic
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${Cg_LIBRARIES}
	${awl_TARGET}
	${OPENCL_LIBRARIES}
	${OPENGL_gl_LIBRARY}
	${sge_TARGETS}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_solipsistic
	DESTINATION
	${INSTALL_BINARY_DIR})
## solipsistic end
#
## scan begin
awl_utils_add_portable_executable(
	tests_scan
	"custom_main"
	src/tests/scan.cpp)

target_link_libraries(
	tests_scan
	${FLAKELIB_DEFAULT_LINK_TARGET}
	flake_helper_library
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${awl_TARGET}
	${OPENGL_gl_LIBRARY}
	${OPENCL_LIBRARIES}
	${sge_TARGETS}
	${Cg_LIBRARIES}
	${Boost_LIBRARIES})

install(
	TARGETS
	tests_scan
	DESTINATION
	${INSTALL_BINARY_DIR})
# scan end
