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

fcppt_utils_set_target_compiler_flags(
	tests_vorticity)

target_link_libraries(
	tests_vorticity
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
# vorticity end

# buoyancy begin
awl_utils_add_portable_executable(
	tests_buoyancy
	"custom_main"
	src/tests/buoyancy.cpp)

fcppt_utils_set_target_compiler_flags(
	tests_buoyancy)

target_link_libraries(
	tests_buoyancy
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
# buoyancy end
#
# flakes begin
awl_utils_add_portable_executable(
	tests_flakes
	"custom_main"
	src/tests/flakes.cpp)

fcppt_utils_set_target_compiler_flags(
	tests_flakes)

target_link_libraries(
	tests_flakes
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
# flakes end
#
## marching_cubes begin
awl_utils_add_portable_executable(
	tests_marching_cubes
	"custom_main"
	src/tests/marching_cubes.cpp)

fcppt_utils_set_target_compiler_flags(
	tests_marching_cubes)

target_link_libraries(
	tests_marching_cubes
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
## marching_cubes end
#
## density_visualization begin
awl_utils_add_portable_executable(
	tests_density_visualization
	"custom_main"
	src/tests/density_visualization.cpp)

fcppt_utils_set_target_compiler_flags(
	tests_density_visualization)

target_link_libraries(
	tests_density_visualization
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
## density_visualization end
#
#
## solipsistic begin
awl_utils_add_portable_executable(
	tests_solipsistic
	"custom_main"
	src/tests/solipsistic.cpp)

fcppt_utils_set_target_compiler_flags(
	tests_solipsistic)

target_link_libraries(
	tests_solipsistic
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${fcppt_log_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
## solipsistic end
#
## scan begin
awl_utils_add_portable_executable(
	tests_scan
	"custom_main"
	src/tests/scan.cpp)

fcppt_utils_set_target_compiler_flags(
	tests_scan)

target_link_libraries(
	tests_scan
	PRIVATE
	${flakelib_TARGET}
	${flakelib_helper_library_TARGET}
	${fcppt_core_TARGET}
	${fcppt_filesystem_TARGET}
	${fcppt_log_TARGET}
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
	${CMAKE_INSTALL_BINDIR})
# scan end
