#include <flakelib/duration.hpp>
#include <flakelib/media_path.hpp>
#include <flakelib/media_path_from_string.hpp>
#include <flakelib/utf8_file_to_fcppt_string.hpp>
#include <flakelib/laplace_solver/dynamic_factory.hpp>
#include <flakelib/planar_pool/object.hpp>
#include <flakelib/simulation/stam/object.hpp>
#include <flakelib/utility/object.hpp>
#include <sge/image/capabilities_field.hpp>
#include <sge/image2d/file.hpp>
#include <sge/image2d/file_ptr.hpp>
#include <sge/image2d/system.hpp>
#include <sge/image2d/view/const_object.hpp>
#include <sge/log/global_context.hpp>
#include <sge/media/all_extensions.hpp>
#include <sge/opencl/single_device_system.hpp>
#include <sge/opencl/memory_object/create_image_format.hpp>
#include <sge/parse/json/array.hpp>
#include <sge/parse/json/find_and_convert_member.hpp>
#include <sge/parse/json/object.hpp>
#include <sge/parse/json/parse_string_exn.hpp>
#include <sge/parse/json/path.hpp>
#include <sge/parse/json/string_to_path.hpp>
#include <sge/parse/json/config/create_command_line_parameters.hpp>
#include <sge/parse/json/config/merge_command_line_parameters.hpp>
#include <sge/systems/image2d.hpp>
#include <sge/systems/instance.hpp>
#include <sge/systems/list.hpp>
#include <fcppt/exception.hpp>
#include <fcppt/text.hpp>
#include <fcppt/to_std_string.hpp>
#include <fcppt/container/bitfield/basic_impl.hpp>
#include <fcppt/filesystem/path_to_string.hpp>
#include <fcppt/io/cerr.hpp>
#include <fcppt/io/cout.hpp>
#include <fcppt/log/activate_levels.hpp>
#include <fcppt/log/context.hpp>
#include <fcppt/log/level.hpp>
#include <fcppt/log/location.hpp>
#include <fcppt/math/dim/basic_impl.hpp>
#include <fcppt/config/external_begin.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <ostream>
#include <fcppt/config/external_end.hpp>


int
main(
	int argc,
	char *argv[])
try
{
	sge::log::global_context().apply(
		fcppt::log::location(
			FCPPT_TEXT("opencl")),
		std::tr1::bind(
			&fcppt::log::activate_levels,
			std::tr1::placeholders::_1,
			fcppt::log::level::verbose));

	sge::parse::json::object const config_file =
		sge::parse::json::config::merge_command_line_parameters(
			sge::parse::json::parse_string_exn(
				flakelib::utf8_file_to_fcppt_string(
					flakelib::media_path_from_string(
						FCPPT_TEXT("config.json")))),
			sge::parse::json::config::create_command_line_parameters(
				argc,
				argv));

	sge::opencl::single_device_system opencl_system(
		(sge::opencl::optional_renderer()),
		(sge::opencl::context::optional_error_callback()));

	flakelib::build_options global_build_options(
		std::string(
			"-cl-mad-enable -cl-fast-relaxed-math -Werror ")+
		"-I "+
		fcppt::to_std_string(
			fcppt::filesystem::path_to_string(
				flakelib::media_path_from_string(
					FCPPT_TEXT("kernels")))));

	sge::systems::instance sys(
		sge::systems::list()
			(sge::systems::image2d(
				sge::image::capabilities_field::null(),
				sge::media::all_extensions)));

	sge::image2d::file_ptr boundary_image =
		sys.image_system().load(
			flakelib::media_path()
				/ FCPPT_TEXT("images")
				/
				sge::parse::json::find_and_convert_member<fcppt::string>(
					config_file,
					sge::parse::json::string_to_path(FCPPT_TEXT("stam-test/boundary-file"))));

	flakelib::planar_pool::object scalar_pool(
		opencl_system.context(),
		sge::opencl::memory_object::create_image_format(
			CL_R,
			CL_FLOAT));

	flakelib::planar_pool::object arrow_pool(
		opencl_system.context(),
		sge::opencl::memory_object::create_image_format(
			CL_RG,
			CL_FLOAT));

	flakelib::utility::object utility_object(
		opencl_system.command_queue(),
		global_build_options);

	flakelib::laplace_solver::dynamic_factory configurable_solver(
		scalar_pool,
		opencl_system.command_queue(),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("simulation-solver"))),
		global_build_options,
		utility_object);

	flakelib::simulation::stam::object simulation(
		opencl_system.command_queue(),
		flakelib::boundary_view(
			boundary_image->view()),
		sge::parse::json::find_and_convert_member<sge::parse::json::object>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("stam-test"))),
		global_build_options,
		flakelib::simulation::arrow_image_cache(
			arrow_pool),
		flakelib::simulation::scalar_image_cache(
			scalar_pool),
		utility_object,
		configurable_solver.value());

	unsigned const benchmark_iterations =
		sge::parse::json::find_and_convert_member<unsigned>(
			config_file,
			sge::parse::json::string_to_path(
				FCPPT_TEXT("benchmark-iterations")));

	fcppt::io::cout() << FCPPT_TEXT("Starting benchmark...\n");
	for(unsigned i = 0; i < benchmark_iterations; ++i)
	{
		simulation.update(
			flakelib::duration(
				0.01f));
	}
	fcppt::io::cout() << FCPPT_TEXT("Done!\n");
}
catch(fcppt::exception const &e)
{
	fcppt::io::cerr() << FCPPT_TEXT("fcppt exception caught: ") << e.string() << FCPPT_TEXT("\n");
	return EXIT_FAILURE;
}
catch(std::exception const &e)
{
	std::cerr << "standard exception caught: " << e.what() << "\n";
	return EXIT_FAILURE;
}
catch(...)
{
	std::cerr << "unknown exception caught\n";
	return EXIT_FAILURE;
}

